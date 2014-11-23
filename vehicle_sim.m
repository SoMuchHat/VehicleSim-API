function [ output_args ] = vehicle_sim(sim_file, procedures, mods, costFunction)

    % Sim_file should be a filename as a string
    % Procedures should be an array of procedure file names as strings
    
    % Mods: Key-value pairings in a container.Map structure
    % Calls the function costFunction if costFunction is a string
    
    
    if libisloaded('vs_solver')
        unloadlibrary('vs_solver');
    end 
    
    SolverPath = vs_dll_path(sim_file);
    disp(SolverPath)
    %Load the solver DLL
    [notfound, warnings] = ...
        loadlibrary(SolverPath, 'vs_api_def_m.h', 'alias', 'vs_solver');
    disp('VS Solver DLL loaded and the simulator is now running..')
    
    % Used to view functions available (TESTING ONLY):
    libfunctions('vs_solver', '-full')
    libfunctionsview('vs_solve')
    
    % Starting the run
    calllib('vs_solver', 'vs_run', sim_file);
    
    %feval(costFunction)
     
    % Unload DLL
    unloadlibrary('vs_solver');
     
end