CSV files are prototype/demo campus data.
locations.csv: id,name,type,floor
pathways.csv: source_id,source,destination_id,destination,distance_m,walking_time_min,steps,stairs,elevator,ramp,blocked
indoor_locations.csv: id,building_id,building_name,floor,room_name,type
indoor_pathways.csv: source_id,destination_id,distance_m,walking_time_min,stairs,elevator,ramp,blocked
building_info.csv: building metadata.
Route history is written to route_history.txt when routes are found.
