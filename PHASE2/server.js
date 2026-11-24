const http = require('http');
const fs = require('fs');
const path = require('path');
const { spawnSync } = require('child_process');

const ROOT = __dirname;
const FRONTEND = path.join(ROOT, 'frontend');
const DATA = path.join(ROOT, 'data');
const PORT = 3000;
const EXE = path.join(ROOT, 'CampusNavX.exe');

function compileCpp() {
    console.log('Compiling CampusNavX C++ backend...');

    const result = spawnSync(
        'g++',
        ['main.cpp', '-std=c++11', '-o', 'CampusNavX.exe'],
        {
            cwd: ROOT,
            encoding: 'utf8',
            windowsHide: true
        }
    );

    if (result.error || result.status !== 0) {
        console.error(
            (result.stderr || '') +
            (result.stdout || '') +
            (result.error ? result.error.message : '')
        );
        return false;
    }

    console.log('C++ backend compiled successfully.');
    return true;
}

function runCpp(args) {
    if (!fs.existsSync(EXE)) {
        return {
            success: false,
            output: 'CampusNavX.exe not found. Compile main.cpp first.'
        };
    }

    const result = spawnSync(EXE, args, {
        cwd: ROOT,
        encoding: 'utf8',
        timeout: 15000,
        windowsHide: true
    });

    if (result.error) {
        return {
            success: false,
            output: result.error.message
        };
    }

    return {
        success: result.status === 0,
        output: (result.stdout || '') + (result.stderr || '')
    };
}

function readCSV(filename) {
    const file = path.join(DATA, filename);

    if (!fs.existsSync(file)) {
        return [];
    }

    const lines = fs.readFileSync(file, 'utf8')
        .replace(/^\uFEFF/, '')
        .split(/\r?\n/)
        .filter(x => x.trim());

    if (lines.length < 2) {
        return [];
    }

    const headers = lines[0].split(',').map(x => x.trim());

    return lines.slice(1).map(line => {
        const values = line.split(',');
        const item = {};

        headers.forEach((header, i) => {
            item[header] = (values[i] || '').trim();
        });

        return item;
    });
}

function mapData() {
    const outdoorLocations = readCSV('locations.csv');
    const indoorLocations = readCSV('indoor_locations.csv');

    const outdoorEdges = readCSV('pathways.csv');
    const indoorEdges = readCSV('indoor_pathways.csv');

    const outdoorStatus = readCSV('outdoor_pathway_status.csv');
    const indoorStatus = readCSV('indoor_pathway_status.csv');

    function applyStatus(edges, statuses) {
        return edges.map(edge => {
            const status = statuses.find(s =>
                (s.source === edge.source &&
                 s.destination === edge.destination) ||
                (s.source === edge.destination &&
                 s.destination === edge.source)
            );

            return Object.assign({}, edge, {
                blocked: status
                    ? status.blocked
                    : (edge.blocked || '0')
            });
        });
    }

    return {
        outdoor: {
            locations: outdoorLocations,
            edges: applyStatus(outdoorEdges, outdoorStatus)
        },

        indoor: {
            locations: indoorLocations,
            edges: applyStatus(indoorEdges, indoorStatus)
        }
    };
}

function sendJSON(res, status, data) {
    res.writeHead(status, {
        'Content-Type': 'application/json; charset=utf-8',
        'Cache-Control': 'no-store'
    });

    res.end(JSON.stringify(data));
}

function readBody(req) {
    return new Promise((resolve, reject) => {
        let body = '';

        req.on('data', chunk => {
            body += chunk;

            if (body.length > 100000) {
                reject(new Error('Request too large'));
                req.destroy();
            }
        });

        req.on('end', () => {
            try {
                resolve(JSON.parse(body || '{}'));
            } catch (error) {
                reject(error);
            }
        });

        req.on('error', reject);
    });
}

function serveFrontend(req, res) {
    let requested = new URL(req.url, 'http://localhost').pathname;

    if (requested === '/') {
        requested = '/index.html';
    }

    let decoded;

    try {
        decoded = decodeURIComponent(requested);
    } catch (_) {
        decoded = '/';
    }

    const filePath = path.resolve(FRONTEND, '.' + decoded);

    if (!filePath.startsWith(FRONTEND + path.sep)) {
        res.writeHead(403);
        res.end('Forbidden');
        return;
    }

    if (!fs.existsSync(filePath) || !fs.statSync(filePath).isFile()) {
        res.writeHead(404);
        res.end('File not found');
        return;
    }

    const types = {
        '.html': 'text/html; charset=utf-8',
        '.css': 'text/css; charset=utf-8',
        '.js': 'application/javascript; charset=utf-8',
        '.png': 'image/png',
        '.jpg': 'image/jpeg',
        '.svg': 'image/svg+xml'
    };

    res.writeHead(200, {
        'Content-Type':
            types[path.extname(filePath).toLowerCase()] ||
            'application/octet-stream'
    });

    fs.createReadStream(filePath).pipe(res);
}

const server = http.createServer(async (req, res) => {
    const url = new URL(req.url, 'http://localhost');

    if (req.method === 'OPTIONS') {
        res.writeHead(204, {
            'Access-Control-Allow-Origin': '*',
            'Access-Control-Allow-Methods': 'GET, POST, OPTIONS',
            'Access-Control-Allow-Headers': 'Content-Type'
        });

        res.end();
        return;
    }

    try {
        if (url.pathname === '/api/health') {
            return sendJSON(res, 200, {
                success: fs.existsSync(EXE),
                message: fs.existsSync(EXE)
                    ? 'C++ backend connected'
                    : 'C++ executable not found'
            });
        }

        if (url.pathname === '/api/locations-list') {
            return sendJSON(res, 200, {
                outdoor: readCSV('locations.csv'),
                indoor: readCSV('indoor_locations.csv')
            });
        }

        if (url.pathname === '/api/map-data' && req.method === 'GET') {
            return sendJSON(res, 200, mapData());
        }

        if (url.pathname === '/api/locations' && req.method === 'GET') {
            return sendJSON(
                res,
                200,
                runCpp([
                    'locations',
                    url.searchParams.get('type') || 'outdoor'
                ])
            );
        }

        if (url.pathname === '/api/route' && req.method === 'POST') {
            const body = await readBody(req);

            if (!body.start || !body.destination) {
                return sendJSON(res, 400, {
                    success: false,
                    output: 'Please select both locations.'
                });
            }

            return sendJSON(
                res,
                200,
                runCpp([
                    'route',
                    String(body.start),
                    String(body.destination),
                    body.accessible ? '1' : '0'
                ])
            );
        }

        if (url.pathname === '/api/pathway' && req.method === 'POST') {
            const body = await readBody(req);

            if (
                !body.type ||
                !body.action ||
                !body.first ||
                !body.second
            ) {
                return sendJSON(res, 400, {
                    success: false,
                    output: 'Please complete all pathway details.'
                });
            }

            if (
                !['indoor', 'outdoor'].includes(body.type) ||
                !['block', 'unblock'].includes(body.action)
            ) {
                return sendJSON(res, 400, {
                    success: false,
                    output: 'Invalid pathway details.'
                });
            }

            const result = runCpp([
                'pathway',
                body.type,
                body.action,
                String(body.first),
                String(body.second)
            ]);

            return sendJSON(
                res,
                200,
                Object.assign({}, result, {
                    map: mapData()
                })
            );
        }

        if (url.pathname === '/api/history' && req.method === 'GET') {
            return sendJSON(res, 200, runCpp(['history']));
        }

        if (req.method === 'GET') {
            return serveFrontend(req, res);
        }

        return sendJSON(res, 404, {
            success: false,
            output: 'API endpoint not found.'
        });

    } catch (error) {
        return sendJSON(res, 500, {
            success: false,
            output: error.message
        });
    }
});

if (!compileCpp()) {
    console.log(
        'Please fix the C++ compilation errors before starting the dashboard.'
    );

    process.exit(1);
}

server.listen(PORT, () => {
    console.log(
        '\nCampusNavX Dashboard\n' +
        'Frontend: http://localhost:' + PORT +
        '\nC++ backend: Connected\n'
    );
});