function [ output_args ] = vehicle_sim(base_vehicle, procedures, mods, costFunction)
    
    % Base vehicle should be a filename as a string
    
    % Procedures should be an array of procedure file names as strings

    % Mods: Key-value pairings in a container.Map structure
    % Calls the function costFunction if costFunction is a string
    feval(costFunction)
end