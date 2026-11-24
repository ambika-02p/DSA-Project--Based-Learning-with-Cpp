CampusNavX CSV sample data
===========================

Files:
- locations.csv: outdoor campus locations
- pathways.csv: outdoor links between locations
- indoor_locations.csv: indoor rooms, corridors, floors, labs and entrances
- indoor_pathways.csv: links between indoor locations
- building_connectors.csv: connects outdoor building nodes to indoor entrances

Important:
- This is illustrative sample data, not verified campus measurements.
- Distance values are sample distances in metres.
- travel_time values are sample minutes.
- blocked: 0 = open, 1 = blocked.
- stairs: 0 = no stairs, 1 = includes stairs.
- IDs in pathway files refer to IDs in the corresponding location file.
- These CSVs need matching C++ loading code and graph classes to be used by the program.
