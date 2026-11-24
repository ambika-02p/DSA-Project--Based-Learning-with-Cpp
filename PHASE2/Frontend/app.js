const API = '';

let currentMapType = 'outdoor';
let mapData = null;
let currentRouteSegments = [];

// ---------------- BASIC HELPERS ----------------

const $ = id => document.getElementById(id);

async function apiRequest(url, options = {}) {
    const response = await fetch(API + url, {
        headers: {
            'Content-Type': 'application/json',
            ...(options.headers || {})
        },
        ...options
    });

    let data;

    try {
        data = await response.json();
    } catch {
        throw new Error('Invalid response from server.');
    }

    if (!response.ok) {
        throw new Error(
            data.output || data.message || 'Request failed'
        );
    }

    return data;
}

function showOutput(id, value) {
    const element = $(id);

    if (element) {
        element.textContent = value || 'No information available.';
    }
}

function setStatus(connected) {
    if (!$('statusDot')) return;

    $('statusDot').className =
        connected ? 'connected' : 'disconnected';

    $('statusTitle').textContent =
        connected ? 'Backend Connected' : 'Backend Offline';

    $('statusText').textContent =
        connected ? 'C++ system is running' : 'Check server connection';
}

function setRouteStatus(message, error = false) {
    if (!$('routeStatus')) return;

    $('routeStatus').textContent = message;
    $('routeStatus').classList.toggle('error', error);
}

function clearCurrentRoute() {
    currentRouteSegments = [];

    if ($('routeEdges')) {
        $('routeEdges').innerHTML = '';
    }

    document.querySelectorAll('.map-node').forEach(node => {
        node.classList.remove('route-node');
    });
}


// ---------------- PAGE NAVIGATION ----------------

document.querySelectorAll('.nav-link').forEach(button => {
    button.addEventListener('click', () => {
        const page = button.dataset.page;

        document.querySelectorAll('.nav-link').forEach(item => {
            item.classList.remove('active');
        });

        document.querySelectorAll('.page').forEach(item => {
            item.classList.remove('active');
        });

        button.classList.add('active');

        if ($(page)) {
            $(page).classList.add('active');
        }

        const titles = {
            navigate: 'Campus Navigation',
            locations: 'Campus Locations',
            pathways: 'Pathway Control',
            history: 'Route History'
        };

        $('pageTitle').textContent =
            titles[page] || 'Campus Navigation';
    });
});


// ---------------- LOCATION DATA ----------------

function getLocationName(item) {
    return (
        item.name ||
        item.location_name ||
        item.location ||
        item.room ||
        item.building ||
        item.source ||
        ''
    );
}

function populateSelect(select, locations, placeholder) {
    if (!select) return;

    select.innerHTML = '';

    const first = document.createElement('option');
    first.value = '';
    first.textContent = placeholder;

    select.appendChild(first);

    locations.forEach(item => {
        const name = getLocationName(item);

        if (!name) return;

        const option = document.createElement('option');

        option.value = name;
        option.textContent = name;

        select.appendChild(option);
    });
}

function getLocations(type) {
    if (!mapData) return [];

    if (type === 'indoor') {
        return mapData.indoor?.locations || [];
    }

    return mapData.outdoor?.locations || [];
}

function updateLocationDropdowns() {
    const type = $('routeType').value;
    const locations = getLocations(type);

    populateSelect(
        $('startLocation'),
        locations,
        'Select starting location'
    );

    populateSelect(
        $('endLocation'),
        locations,
        'Select destination'
    );
}

function updatePathwayDropdowns() {
    const type = $('pathwayType').value;
    const locations = getLocations(type);

    populateSelect(
        $('pathwayFirst'),
        locations,
        'Select first location'
    );

    populateSelect(
        $('pathwaySecond'),
        locations,
        'Select second location'
    );
}


// ---------------- MAP DATA HELPERS ----------------

function getCoordinates(item, index, total) {
    const xValue =
        item.x ?? item.pos_x ?? item.longitude ?? item.map_x;

    const yValue =
        item.y ?? item.pos_y ?? item.latitude ?? item.map_y;

    if (
        xValue !== undefined &&
        yValue !== undefined &&
        xValue !== '' &&
        yValue !== ''
    ) {
        const x = Number(xValue);
        const y = Number(yValue);

        if (Number.isFinite(x) && Number.isFinite(y)) {
            return {
                x: Math.max(45, Math.min(955, x)),
                y: Math.max(45, Math.min(555, y))
            };
        }
    }

    // Generate schematic positions when CSV coordinates are absent.

    const columns = Math.max(2, Math.ceil(Math.sqrt(total)));
    const rows = Math.ceil(total / columns);

    const column = index % columns;
    const row = Math.floor(index / columns);

    return {
        x: 100 + column * (800 / Math.max(1, columns - 1)),
        y: 100 + row * (400 / Math.max(1, rows - 1))
    };
}

function getNodeId(item) {
    return String(
        item.id ??
        item.location_id ??
        item.node_id ??
        item.source_id ??
        ''
    );
}

function getEdgeSource(edge) {
    return String(
        edge.source ??
        edge.source_id ??
        edge.from ??
        ''
    );
}

function getEdgeDestination(edge) {
    return String(
        edge.destination ??
        edge.destination_id ??
        edge.to ??
        ''
    );
}

function findLocation(locations, identifier) {
    return locations.find(item => {
        const name = getLocationName(item);

        return (
            getNodeId(item) === String(identifier) ||
            name.toLowerCase() === String(identifier).toLowerCase()
        );
    });
}

function createSVGElement(name, attributes = {}) {
    const element = document.createElementNS(
        'http://www.w3.org/2000/svg',
        name
    );

    Object.entries(attributes).forEach(([key, value]) => {
        element.setAttribute(key, value);
    });

    return element;
}

function isBlocked(edge) {
    return (
        edge.blocked === true ||
        Number(edge.blocked) === 1 ||
        String(edge.blocked).toLowerCase() === 'true'
    );
}

function getMapEdges(type) {
    if (!mapData) return [];

    return type === 'indoor'
        ? mapData.indoor?.edges || []
        : mapData.outdoor?.edges || [];
}


// ---------------- DRAW MAP ----------------

function drawMap(type = currentMapType) {
    if (!mapData) return;

    currentMapType = type;

    const locations = getLocations(type);
    const edges = getMapEdges(type);

    const edgeGroup = $('mapEdges');
    const routeGroup = $('routeEdges');
    const nodeGroup = $('mapNodes');

    if (!edgeGroup || !routeGroup || !nodeGroup) return;

    edgeGroup.innerHTML = '';
    routeGroup.innerHTML = '';
    nodeGroup.innerHTML = '';

    $('mapMessage').textContent = '';

    if (!locations.length) {
        $('mapMessage').textContent =
            'No locations found in CSV data.';
        return;
    }

    const positions = new Map();

    locations.forEach((item, index) => {
        positions.set(
            getNodeId(item) || getLocationName(item),
            getCoordinates(item, index, locations.length)
        );
    });

    // Draw pathways.

    edges.forEach(edge => {
        const source = findLocation(
            locations,
            getEdgeSource(edge)
        );

        const destination = findLocation(
            locations,
            getEdgeDestination(edge)
        );

        if (!source || !destination) return;

        const sourcePosition = positions.get(
            getNodeId(source) || getLocationName(source)
        );

        const destinationPosition = positions.get(
            getNodeId(destination) || getLocationName(destination)
        );

        if (!sourcePosition || !destinationPosition) return;

        const line = createSVGElement('line', {
            x1: sourcePosition.x,
            y1: sourcePosition.y,
            x2: destinationPosition.x,
            y2: destinationPosition.y,
            class: isBlocked(edge)
                ? 'map-edge blocked'
                : 'map-edge'
        });

        edgeGroup.appendChild(line);
    });

    // Draw locations.

    locations.forEach(item => {
        const key = getNodeId(item) || getLocationName(item);
        const position = positions.get(key);

        const isBuilding =
            String(item.type || '').toLowerCase().includes('building');

        const group = createSVGElement('g', {
            class: 'map-node ' +
                (isBuilding ? 'building' : 'facility')
        });

        const circle = createSVGElement('circle', {
            cx: position.x,
            cy: position.y,
            r: 10
        });

        const label = createSVGElement('text', {
            x: position.x + 13,
            y: position.y - 12
        });

        label.textContent = getLocationName(item);

        group.appendChild(circle);
        group.appendChild(label);

        nodeGroup.appendChild(group);
    });

    $('outdoorMapButton').classList.toggle(
        'active',
        type === 'outdoor'
    );

    $('indoorMapButton').classList.toggle(
        'active',
        type === 'indoor'
    );

    // Highlight the matching segment of the current route.

    highlightRoute();
}


// ---------------- ROUTE PARSING ----------------

function extractNamesFromLine(line) {
    return line
        .split('->')
        .map(part => part.trim())
        .map(part => part.replace(/^(route|path)\s*:\s*/i, '').trim())
        .filter(Boolean);
}

function extractRouteSegments(output) {
    const lines = output.split(/\r?\n/);

    const segments = [];

    // Cross-building route has three sections.

    if (/COMPLETE ROUTE/i.test(output)) {
        let currentType = null;

        lines.forEach(line => {
            if (/1\.\s*Inside/i.test(line)) {
                currentType = 'indoor';
                return;
            }

            if (/2\.\s*Between buildings/i.test(line)) {
                currentType = 'outdoor';
                return;
            }

            if (/3\.\s*Inside/i.test(line)) {
                currentType = 'indoor';
                return;
            }

            if (line.includes('->') && currentType) {
                const names = extractNamesFromLine(line);

                if (names.length >= 2) {
                    segments.push({
                        type: currentType,
                        names
                    });
                }
            }
        });

        return segments;
    }

    // Same-building indoor route.

    if (/INDOOR ROUTE/i.test(output)) {
        lines.forEach(line => {
            if (line.includes('->')) {
                const names = extractNamesFromLine(line);

                if (names.length >= 2) {
                    segments.push({
                        type: 'indoor',
                        names
                    });
                }
            }
        });

        return segments;
    }

    // Normal outdoor route.

    lines.forEach(line => {
        if (line.includes('->')) {
            const names = extractNamesFromLine(line);

            if (names.length >= 2) {
                segments.push({
                    type: 'outdoor',
                    names
                });
            }
        }
    });

    return segments;
}

function hasNoRoute(output) {
    return (
        /no outdoor route available/i.test(output) ||
        /no indoor route available/i.test(output) ||
        /no complete route found/i.test(output) ||
        /no route available/i.test(output) ||
        /location not found/i.test(output) ||
        /connector not found/i.test(output)
    );
}


// ---------------- ROUTE HIGHLIGHT ----------------

function highlightRoute() {
    if (!mapData) return;

    const routeGroup = $('routeEdges');
    const nodeGroup = $('mapNodes');

    if (!routeGroup || !nodeGroup) return;

    routeGroup.innerHTML = '';

    const locations = getLocations(currentMapType);
    const edges = getMapEdges(currentMapType);

    // Select only route segments belonging to this map.

    const matchingSegments = currentRouteSegments.filter(
        segment => segment.type === currentMapType
    );

    const routeNames = [];

    matchingSegments.forEach(segment => {
        segment.names.forEach(name => {
            if (!routeNames.includes(name.toLowerCase())) {
                routeNames.push(name.toLowerCase());
            }
        });
    });

    const positions = new Map();

    locations.forEach((item, index) => {
        positions.set(
            getNodeId(item) || getLocationName(item),
            getCoordinates(item, index, locations.length)
        );
    });

    // Highlight route edges.

    matchingSegments.forEach(segment => {
        const names = segment.names;

        for (let i = 0; i < names.length - 1; i++) {
            const first = findLocation(locations, names[i]);
            const second = findLocation(locations, names[i + 1]);

            if (!first || !second) continue;

            const firstPosition = positions.get(
                getNodeId(first) || getLocationName(first)
            );

            const secondPosition = positions.get(
                getNodeId(second) || getLocationName(second)
            );

            if (!firstPosition || !secondPosition) continue;

            const firstId = getNodeId(first);
            const secondId = getNodeId(second);

            const firstName = getLocationName(first).toLowerCase();
            const secondName = getLocationName(second).toLowerCase();

            const connected = edges.some(edge => {
                const a = getEdgeSource(edge);
                const b = getEdgeDestination(edge);

                return (
                    (firstId && secondId &&
                        a === firstId && b === secondId) ||
                    (firstId && secondId &&
                        a === secondId && b === firstId) ||
                    (a.toLowerCase() === firstName &&
                        b.toLowerCase() === secondName) ||
                    (a.toLowerCase() === secondName &&
                        b.toLowerCase() === firstName)
                );
            });

            if (!connected) continue;

            const line = createSVGElement('line', {
                x1: firstPosition.x,
                y1: firstPosition.y,
                x2: secondPosition.x,
                y2: secondPosition.y,
                class: 'route-edge'
            });

            routeGroup.appendChild(line);
        }
    });

    // Highlight route nodes.

    Array.from(nodeGroup.children).forEach(group => {
        const label = group.querySelector('text');

        if (!label) return;

        if (routeNames.includes(label.textContent.toLowerCase())) {
            group.classList.add('route-node');
        } else {
            group.classList.remove('route-node');
        }
    });
}


// ---------------- LOAD MAP ----------------

async function loadMap() {
    try {
        mapData = await apiRequest('/api/map-data');

        updateLocationDropdowns();
        updatePathwayDropdowns();

        drawMap(currentMapType);

        setStatus(true);

    } catch (error) {
        setStatus(false);

        if ($('mapMessage')) {
            $('mapMessage').textContent =
                'Unable to load map data. Check server.js and CSV files.';
        }
    }
}


// ---------------- ROUTE SEARCH ----------------

async function findRoute() {
    const start = $('startLocation').value;
    const destination = $('endLocation').value;
    const accessible = $('accessible').checked;

    if (!start || !destination) {
        showOutput(
            'routeOutput',
            'Please select both starting location and destination.'
        );

        setRouteStatus('INCOMPLETE', true);
        return;
    }

    if (start.toLowerCase() === destination.toLowerCase()) {
        showOutput(
            'routeOutput',
            'Start and destination cannot be the same.'
        );

        setRouteStatus('INVALID', true);
        return;
    }

    const button = $('findRoute');

    button.disabled = true;
    button.textContent = 'Finding Route...';

    setRouteStatus('SEARCHING');

    try {
        const result = await apiRequest('/api/route', {
            method: 'POST',

            body: JSON.stringify({
                start,
                destination,
                accessible
            })
        });

        const output = result.output || '';

        showOutput('routeOutput', output);

        const noRoute = hasNoRoute(output);

        const routeSegments = extractRouteSegments(output);

        // Accept a route only when the backend output contains
        // an actual path with at least two connected locations.

        const found =
            !noRoute &&
            routeSegments.some(segment => segment.names.length >= 2);

        if (found) {
            currentRouteSegments = routeSegments;

            setRouteStatus('ROUTE FOUND');

            // For cross-building routes, begin with the
            // starting building's indoor map.

            const hasOutdoor = routeSegments.some(
                segment => segment.type === 'outdoor'
            );

            const hasIndoor = routeSegments.some(
                segment => segment.type === 'indoor'
            );

            if (hasIndoor) {
                drawMap('indoor');
            } else if (hasOutdoor) {
                drawMap('outdoor');
            }

        } else {
            clearCurrentRoute();

            setRouteStatus('NO ROUTE', true);

            drawMap($('routeType').value);
        }

    } catch (error) {
        clearCurrentRoute();

        showOutput('routeOutput', error.message);

        setRouteStatus('ERROR', true);

    } finally {
        button.disabled = false;

        button.innerHTML =
            'Find Best Route <span>→</span>';
    }
}


// ---------------- LOCATIONS ----------------

async function showLocations() {
    try {
        const type = $('locationType').value;

        const result = await apiRequest(
            '/api/locations?type=' + type
        );

        showOutput('locationOutput', result.output);

    } catch (error) {
        showOutput('locationOutput', error.message);
    }
}


// ---------------- PATHWAY CONTROL ----------------

async function changePathway(action) {
    const type = $('pathwayType').value;
    const first = $('pathwayFirst').value;
    const second = $('pathwaySecond').value;

    if (!first || !second) {
        showOutput(
            'pathwayOutput',
            'Please select both pathway locations.'
        );

        return;
    }

    if (first === second) {
        showOutput(
            'pathwayOutput',
            'Please select two different locations.'
        );

        return;
    }

    try {
        const result = await apiRequest('/api/pathway', {
            method: 'POST',

            body: JSON.stringify({
                type,
                action,
                first,
                second
            })
        });

        showOutput('pathwayOutput', result.output);

        // Reload updated pathway status and map.

        await loadMap();

    } catch (error) {
        showOutput('pathwayOutput', error.message);
    }
}


// ---------------- ROUTE HISTORY ----------------

async function showHistory() {
    try {
        const result = await apiRequest('/api/history');

        showOutput('historyOutput', result.output);

    } catch (error) {
        showOutput('historyOutput', error.message);
    }
}


// ---------------- MAP BUTTONS ----------------

// Switching maps should NOT erase the current route.
// This allows the user to view indoor and outdoor
// segments of the same cross-building journey.

$('outdoorMapButton').addEventListener('click', () => {
    drawMap('outdoor');
});

$('indoorMapButton').addEventListener('click', () => {
    drawMap('indoor');
});

// Reset removes the highlighted route.

$('resetMap').addEventListener('click', () => {
    clearCurrentRoute();
    drawMap(currentMapType);
});


// ---------------- EVENT LISTENERS ----------------

$('routeType').addEventListener('change', () => {
    clearCurrentRoute();

    updateLocationDropdowns();

    drawMap($('routeType').value);
});

$('pathwayType').addEventListener(
    'change',
    updatePathwayDropdowns
);

$('findRoute').addEventListener('click', findRoute);

$('showLocations').addEventListener('click', showLocations);

$('blockPath').addEventListener('click', () => {
    changePathway('block');
});

$('unblockPath').addEventListener('click', () => {
    changePathway('unblock');
});

$('showHistory').addEventListener('click', showHistory);


// ---------------- START APPLICATION ----------------

async function startApplication() {
    await loadMap();

    try {
        const health = await apiRequest('/api/health');

        setStatus(health.success);

    } catch (error) {
        setStatus(false);
    }
}

startApplication();