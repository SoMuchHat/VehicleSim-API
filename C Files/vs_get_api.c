/* Functions to load a VS API DLL using GetProcAddress. The functions declared
   in vs_api.h are global, so this only works for a single loaded DLL.
   
   Log:
   May 17, 10. M. Sayers. Complete re-write with vs_get_api, better error handling.
   May 18, 09. M. Sayers. Include vs_get_api_install_external for CarSim 8.0.
   Jun 13, 08. M. Sayers. Created for the release of CarSim 7.1.
   */
   
// Standard C headers.
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

#include "vs_deftypes.h" // VS types and definitions
#include "vs_api.h"  // VS API definitions as prototypes

// Utilities for error handling
static int vss_printf_error (int code, const char *format, ...)
{
  char tmpstr[FILENAME_MAX + 100];

  va_list args;
  va_start (args, format);
  vsprintf (tmpstr, format, args);
  va_end (args);
  
  MessageBox (NULL, tmpstr, "Sorry", MB_ICONERROR);
  return code;
}


/* ----------------------------------------------------------------------------
   Get solver path according keyword DLLFILE in the simfile. Return 0 if OK,
   -1 if not.
---------------------------------------------------------------------------- */
int vs_get_dll_path(char *simfile, char *pathDLL)
{
  FILE *fp;
  char *key, *rest, tmpstr[256];

  if ((fp = fopen(simfile, "r")) == NULL)
    return vss_printf_error(-1, 
              "\nThis program needs a simfile to obtain other file names. The file\n"
              "\"%s\" either does not exist or could not be opened.", simfile);

  // scan simfile one line at a time for keyword "DLLFILE"
  while (fgets(tmpstr, 255, fp))
    {
    key = (char *)strtok(tmpstr, " \t\n");
    rest = (char *)strtok(NULL, "\n\t");
    if (!strcmp(key, "DLLFILE") && rest && rest[0])
      {
      strcpy (pathDLL, rest);
      fclose(fp);
      
      // Now see if the DLL exists
      if ((fp = fopen(pathDLL, "rb")) == NULL)
        return vss_printf_error(-1, 
          "\nThe simfile identified the DLL file \"%s\"\n"
          "with the keyword DLLFILE. This DLL file either does not exist or "
          "cannot be opened.", pathDLL);
 
      fclose(fp);
      return 0;
      }
    else if (!strcmp(key, "END")) break;
    }

  fclose(fp);
  return vss_printf_error(-1, 
           "\nThis program needs a DLL to run, identified with the\n"
           "keyword DLLFILE. The simfile \"%s\" did\n"
           "not identify a DLL file.", simfile);
}

// try to get the address of an API function from a DLL
static int vss_get (void *api_func, HMODULE dll, char *func, char *dll_name,
                            char *get_func)
{
  if (dll == NULL) return -1;
  (*(void**)api_func) = (void *)GetProcAddress(dll, func); // cast to work
  if (*(void**)api_func) return 0;
  return vss_printf_error(-2, 
             "The function %s could not get the requested VS API function \"%s\"\n"
             "from the DLL: \"%s\".", get_func, func, dll_name);

}

// utility to handle error if a DLL isn't there.
static int vss_print_no_dll (char *where, char *dll_name)
{
  return vss_printf_error(-1, "The function %s was not given a valid DLL.\n"
                              "\"%s\" did not load.", where, dll_name);
}


/* ----------------------------------------------------------------------------
   Get all VS API functions declared in "vs_api.h"   
---------------------------------------------------------------------------- */
int vs_get_api (HMODULE dll, char *dname)
{
  char *me = "vs_get_api";
  if (dll == NULL) return vss_print_no_dll(me, dname);

  // simple run function (chapter 2)
  if (vss_get(&vs_run, dll, "vs_run", dname, me)) return -2;

  // managing import/export arrays (chapter 4)
  if (vss_get(&vs_copy_export_vars, dll, "vs_copy_export_vars", dname, me)) return -2;
  if (vss_get(&vs_copy_import_vars, dll, "vs_copy_import_vars", dname, me)) return -2;
  if (vss_get(&vs_copy_io, dll, "vs_copy_io", dname, me)) return -2;
  if (vss_get(&vs_integrate_io, dll, "vs_integrate_io", dname, me)) return -2;
  if (vss_get(&vs_integrate_IO, dll, "vs_integrate_IO", dname, me)) return -2;
  if (vss_get(&vs_read_configuration, dll, "vs_read_configuration", dname, me)) return -2;
  if (vss_get(&vs_scale_import_vars, dll, "vs_scale_import_vars", dname, me)) return -2;
  if (vss_get(&vs_terminate_run, dll, "vs_terminate_run", dname, me)) return -2;

  // utility functions: conditons (chapter 5)
  if (vss_get(&vs_during_event, dll, "vs_during_event", dname, me)) return -2;
  if (vss_get(&vs_error_occurred, dll, "vs_error_occurred", dname, me)) return -2;
  if (vss_get(&vs_get_tstep, dll, "vs_get_tstep", dname, me)) return -2;
  if (vss_get(&vs_opt_pause, dll, "vs_opt_pause", dname, me)) return -2;

  // utility functions: messages (chapter 5)
  if (vss_get(&vs_clear_output_message, dll, "vs_clear_output_message", dname, me)) return -2;
  if (vss_get(&vs_clear_error_message, dll, "vs_clear_error_message", dname, me)) return -2;
  if (vss_get(&vs_get_echofile_name, dll, "vs_get_echofile_name", dname, me)) return -2;
  if (vss_get(&vs_get_endfile_name, dll, "vs_get_endfile_name", dname, me)) return -2;
  if (vss_get(&vs_get_erdfile_name, dll, "vs_get_erdfile_name", dname, me)) return -2;
  if (vss_get(&vs_get_error_message, dll, "vs_get_error_message", dname, me)) return -2;
  if (vss_get(&vs_get_infile_name, dll, "vs_get_infile_name", dname, me)) return -2;
  if (vss_get(&vs_get_logfile_name, dll, "vs_get_logfile_name", dname, me)) return -2;
  if (vss_get(&vs_get_output_message, dll, "vs_get_output_message", dname, me)) return -2;
  if (vss_get(&vs_get_simfile_name, dll, "vs_get_simfile_name", dname, me)) return -2;
  if (vss_get(&vs_get_version_model, dll, "vs_get_version_model", dname, me)) return -2;
  if (vss_get(&vs_get_version_product, dll, "vs_get_version_product", dname, me)) return -2;
  if (vss_get(&vs_get_version_vs, dll, "vs_get_version_vs", dname, me)) return -2;
  if (vss_get(&vs_printf, dll, "vs_printf", dname, me)) return -2;
  if (vss_get(&vs_printf_error, dll, "vs_printf_error", dname, me)) return -2;

  // more detailed control of run (chapter 6)
  if (vss_get(&vs_bar_graph_update, dll, "vs_bar_graph_update", dname, me)) return -2;
  if (vss_get(&vs_free_all, dll, "vs_free_all", dname, me)) return -2;
  if (vss_get(&vs_initialize, dll, "vs_initialize", dname, me)) return -2;
  if (vss_get(&vs_integrate, dll, "vs_integrate", dname, me)) return -2;
  if (vss_get(&vs_integrate_io_2, dll, "vs_integrate_io_2", dname, me)) return -2;
  if (vss_get(&vs_setdef_and_read, dll, "vs_setdef_and_read", dname, me)) return -2;
  if (vss_get(&vs_stop_run, dll, "vs_stop_run", dname, me)) return -2;
  if (vss_get(&vs_terminate, dll, "vs_terminate", dname, me)) return -2;

  // install external functions
  if (vss_get(&vs_install_calc_function, dll, "vs_install_calc_function", dname, me)) return -2;
  if (vss_get(&vs_install_echo_function, dll, "vs_install_echo_function", dname, me)) return -2;
  if (vss_get(&vs_install_setdef_function, dll, "vs_install_setdef_function", dname, me)) return -2;
  if (vss_get(&vs_install_scan_function, dll, "vs_install_scan_function", dname, me)) return -2;
  if (vss_get(&vs_install_free_function, dll, "vs_install_free_function", dname, me)) return -2;

  // functions for interacting with the VS math model (chapter 7)
  if (vss_get(&vs_define_import, dll, "vs_define_import", dname, me)) return -2;
  if (vss_get(&vs_define_indexed_parameter_array, dll, "vs_define_indexed_parameter_array", dname, me)) return -2;
  if (vss_get(&vs_define_output, dll, "vs_define_output", dname, me)) return -2;
  if (vss_get(&vs_define_parameter, dll, "vs_define_parameter", dname, me)) return -2;
  if (vss_get(&vs_define_parameter_int, dll, "vs_define_parameter_int", dname, me)) return -2;
  if (vss_get(&vs_define_units, dll, "vs_define_units", dname, me)) return -2;
  if (vss_get(&vs_define_variable, dll, "vs_define_variable", dname, me)) return -2;
  if (vss_get(&vs_get_sym_attribute, dll, "vs_get_sym_attribute", dname, me)) return -2;
  if (vss_get(&vs_get_var_id, dll, "vs_get_var_id", dname, me)) return -2;
  if (vss_get(&vs_get_var_ptr, dll, "vs_get_var_ptr", dname, me)) return -2;
  if (vss_get(&vs_get_var_ptr_int, dll, "vs_get_var_ptr_int", dname, me)) return -2;
  if (vss_get(&vs_have_keyword_in_database, dll, "vs_have_keyword_in_database", dname, me)) return -2;
  if (vss_get(&vs_import_result, dll, "vs_import_result", dname, me)) return -2;
  if (vss_get(&vs_install_calc_func, dll, "vs_install_calc_func", dname, me)) return -2;
  if (vss_get(&vs_install_symbolic_func, dll, "vs_install_symbolic_func", dname, me)) return -2;
  if (vss_get(&vs_read_next_line, dll, "vs_read_next_line", dname, me)) return -2;
  if (vss_get(&vs_set_stop_run, dll, "vs_set_stop_run", dname, me)) return -2;
  if (vss_get(&vs_set_sym_attribute, dll, "vs_set_sym_attribute", dname, me)) return -2;
  if (vss_get(&vs_set_sym_int, dll, "vs_set_sym_int", dname, me)) return -2;
  if (vss_get(&vs_set_sym_real, dll, "vs_set_sym_real", dname, me)) return -2;
  if (vss_get(&vs_set_units, dll, "vs_set_units", dname, me)) return -2;
  if (vss_get(&vs_string_copy_internal, dll, "vs_string_copy_internal", dname, me)) return -2;
  if (vss_get(&vs_write_f_to_echo_file, dll, "vs_write_f_to_echo_file", dname, me)) return -2;
  if (vss_get(&vs_write_header_to_echo_file, dll, "vs_write_header_to_echo_file", dname, me)) return -2;
  if (vss_get(&vs_write_i_to_echo_file, dll, "vs_write_i_to_echo_file", dname, me)) return -2;
  if (vss_get(&vs_write_to_echo_file, dll, "vs_write_to_echo_file", dname, me)) return -2;
  if (vss_get(&vs_write_to_logfile, dll, "vs_write_to_logfile", dname, me)) return -2;

  // 3D road properties (chapter 7)
  if (vss_get(&vs_get_dzds_dzdl, dll, "vs_get_dzds_dzdl", dname, me)) return -2;
  if (vss_get(&vs_get_dzds_dzdl_i, dll, "vs_get_dzds_dzdl_i", dname, me)) return -2;
  if (vss_get(&vs_get_road_contact, dll, "vs_get_road_contact", dname, me)) return -2;
  if (vss_get(&vs_get_road_contact_sl, dll, "vs_get_road_contact_sl", dname, me)) return -2;
  if (vss_get(&vs_get_road_start_stop, dll, "vs_get_road_start_stop", dname, me)) return -2;
  if (vss_get(&vs_get_road_xy_j, dll, "vs_get_road_xy_j", dname, me)) return -2;
  if (vss_get(&vs_get_road_xyz, dll, "vs_get_road_xyz", dname, me)) return -2;
  if (vss_get(&vs_road_curv_i, dll, "vs_road_curv_i", dname, me)) return -2;
  if (vss_get(&vs_road_curv_j, dll, "vs_road_curv_j", dname, me)) return -2;
  if (vss_get(&vs_road_l, dll, "vs_road_l", dname, me)) return -2;
  if (vss_get(&vs_road_l_i, dll, "vs_road_l_i", dname, me)) return -2;
  if (vss_get(&vs_road_pitch_sl_i, dll, "vs_road_pitch_sl_i", dname, me)) return -2;
  if (vss_get(&vs_road_roll_sl_i, dll, "vs_road_roll_sl_i", dname, me)) return -2;
  if (vss_get(&vs_road_s, dll, "vs_road_s", dname, me)) return -2;
  if (vss_get(&vs_road_s_i, dll, "vs_road_s_i", dname, me)) return -2;
  if (vss_get(&vs_road_x, dll, "vs_road_x", dname, me)) return -2;
  if (vss_get(&vs_road_x_i, dll, "vs_road_x_i", dname, me)) return -2;
  if (vss_get(&vs_road_x_sl_i, dll, "vs_road_x_sl_i", dname, me)) return -2;
  if (vss_get(&vs_road_y, dll, "vs_road_y", dname, me)) return -2;
  if (vss_get(&vs_road_y_i, dll, "vs_road_y_i", dname, me)) return -2;
  if (vss_get(&vs_road_y_sl_i, dll, "vs_road_y_sl_i", dname, me)) return -2;
  if (vss_get(&vs_road_yaw, dll, "vs_road_yaw", dname, me)) return -2;
  if (vss_get(&vs_road_yaw_i, dll, "vs_road_yaw_i", dname, me)) return -2;
  if (vss_get(&vs_road_yaw_j, dll, "vs_road_yaw_j", dname, me)) return -2;
  if (vss_get(&vs_road_z, dll, "vs_road_z", dname, me)) return -2;
  if (vss_get(&vs_road_z_i, dll, "vs_road_z_i", dname, me)) return -2;
  if (vss_get(&vs_road_z_sl_i, dll, "vs_road_z_sl_i", dname, me)) return -2;
  if (vss_get(&vs_s_loop, dll, "vs_s_loop", dname, me)) return -2;
  if (vss_get(&vs_target_heading, dll, "vs_target_heading", dname, me)) return -2;
  if (vss_get(&vs_target_l, dll, "vs_target_l", dname, me)) return -2;

  // low-level functions involving the 3D road model
  if (vss_get(&vs_get_road_xy_j, dll, "vs_get_road_xy_j", dname, me)) return -2;
  if (vss_get(&vs_road_curv_j, dll, "vs_road_curv_j", dname, me)) return -2;
  if (vss_get(&vs_road_yaw_j, dll, "vs_road_yaw_j", dname, me)) return -2;  

  // moving objects and sensors (chapter 7)
  if (vss_get(&vs_define_moving_objects, dll, "vs_define_moving_objects", dname, me)) return -2;  
  if (vss_get(&vs_define_sensors, dll, "vs_define_sensors", dname, me)) return -2;  
  if (vss_get(&vs_free_sensors_and_objects, dll, "vs_free_sensors_and_objects", dname, me)) return -2;  
  if (vss_get(&vs_get_n_export_sensor, dll, "vs_get_n_export_sensor", dname, me)) return -2;  
  if (vss_get(&vs_get_sensor_connections, dll, "vs_get_sensor_connections", dname, me)) return -2;  
  
  // configurable table functions (chapter 7)
  if (vss_get(&vs_define_table, dll, "vs_define_table", dname, me)) return -2;  
  if (vss_get(&vs_table_calc, dll, "vs_table_calc", dname, me)) return -2;  
  if (vss_get(&vs_table_index, dll, "vs_table_index", dname, me)) return -2;  
  if (vss_get(&vs_table_ntab, dll, "vs_table_ntab", dname, me)) return -2;  
  if (vss_get(&vs_table_ninst, dll, "vs_table_ninst", dname, me)) return -2;  

  if (vss_get(&vs_copy_table_data, dll, "vs_copy_table_data", dname, me)) return -2;  
  if (vss_get(&vs_install_keyword_tab_group, dll, "vs_install_keyword_tab_group", dname, me)) return -2;  
  if (vss_get(&vs_malloc_table_data, dll, "vs_malloc_table_data", dname, me)) return -2;  

  // saving and restoring the model state (chapter 8)
  if (vss_get(&vs_free_saved_states, dll, "vs_free_saved_states", dname, me)) return -2;
  if (vss_get(&vs_get_request_to_restore, dll, "vs_get_request_to_restore", dname, me)) return -2;
  if (vss_get(&vs_get_request_to_save, dll, "vs_get_request_to_save", dname, me)) return -2;
  if (vss_get(&vs_restore_state, dll, "vs_restore_state", dname, me)) return -2;
  if (vss_get(&vs_save_state, dll, "vs_save_state", dname, me)) return -2;
  if (vss_get(&vs_set_request_to_restore, dll, "vs_set_request_to_restore", dname, me)) return -2;
  if (vss_get(&vs_start_save_timer, dll, "vs_start_save_timer", dname, me)) return -2;
  if (vss_get(&vs_stop_save_timer, dll, "vs_stop_save_timer", dname, me)) return -2;
  if (vss_get(&vs_get_saved_state_time, dll, "vs_get_saved_state_time", dname, me)) return -2;

  // managing arrays to support restarts (chapter 8)
  if (vss_get(&vs_copy_all_state_vars_from_array, dll, "vs_copy_all_state_vars_from_array", dname, me)) return -2;
  if (vss_get(&vs_copy_all_state_vars_to_array, dll, "vs_copy_all_state_vars_to_array", dname, me)) return -2;
  if (vss_get(&vs_copy_differential_state_vars_from_array, dll, "vs_copy_differential_state_vars_from_array", dname, me)) return -2;
  if (vss_get(&vs_copy_differential_state_vars_to_array, dll, "vs_copy_differential_state_vars_to_array", dname, me)) return -2;
  if (vss_get(&vs_copy_extra_state_vars_from_array, dll, "vs_target_l", dname, me)) return -2;
  if (vss_get(&vs_copy_extra_state_vars_to_array, dll, "vs_copy_extra_state_vars_to_array", dname, me)) return -2;
  if (vss_get(&vs_get_export_names, dll, "vs_get_export_names", dname, me)) return -2;
  if (vss_get(&vs_get_import_names, dll, "vs_get_import_names", dname, me)) return -2;
  if (vss_get(&vs_n_derivatives, dll, "vs_n_derivatives", dname, me)) return -2;
  if (vss_get(&vs_n_extra_state_variables, dll, "vs_n_extra_state_variables", dname, me)) return -2;

  // undocumented
  if (vss_get(&vs_get_lat_pos_of_edge, dll, "vs_get_lat_pos_of_edge", dname, me)) return -2;
  if (vss_get(&vs_scale_export_vars, dll, "vs_scale_export_vars", dname, me)) return -2;

  return 0;
}

/* ----------------------------------------------------------------------------
   Legacy get functions for subsets of the API functions.
---------------------------------------------------------------------------- */
int vs_get_api_basic (HMODULE dll, char *dname)
{
  char *me = "vs_get_api_basic";
  if (dll == NULL) return vss_print_no_dll(me, dname);

  if (vss_get(&vs_bar_graph_update, dll, "vs_bar_graph_update", dname, me)) return -2;
  if (vss_get(&vs_copy_io, dll, "vs_copy_io", dname, me)) return -2;
  if (vss_get(&vs_error_occurred, dll, "vs_error_occurred", dname, me)) return -2;
  if (vss_get(&vs_free_all, dll, "vs_free_all", dname, me)) return -2;
  if (vss_get(&vs_get_error_message, dll, "vs_get_error_message", dname, me)) return -2;
  if (vss_get(&vs_get_output_message, dll, "vs_get_output_message", dname, me)) return -2;
  if (vss_get(&vs_get_tstep, dll, "vs_get_tstep", dname, me)) return -2;
  if (vss_get(&vs_get_version_product, dll, "vs_get_version_product", dname, me)) return -2;
  if (vss_get(&vs_get_version_vs, dll, "vs_get_version_vs", dname, me)) return -2;
  if (vss_get(&vs_initialize, dll, "vs_initialize", dname, me)) return -2;
  if (vss_get(&vs_integrate, dll, "vs_integrate", dname, me)) return -2;
  if (vss_get(&vs_integrate_io, dll, "vs_integrate_io", dname, me)) return -2;
  if (vss_get(&vs_opt_pause, dll, "vs_opt_pause", dname, me)) return -2;
  if (vss_get(&vs_read_configuration, dll, "vs_read_configuration", dname, me)) return -2;
  if (vss_get(&vs_setdef_and_read, dll, "vs_setdef_and_read", dname, me)) return -2;
  if (vss_get(&vs_stop_run, dll, "vs_stop_run", dname, me)) return -2;
  if (vss_get(&vs_terminate, dll, "vs_terminate", dname, me)) return -2;
 
  return 0;
}


int vs_get_api_extend (HMODULE dll, char *dname)
{
  char *me = "vs_get_api_extend";
  if (dll == NULL) return vss_print_no_dll(me, dname);

  if (vss_get(&vs_define_import, dll, "vs_define_import", dname, me)) return -2;
  if (vss_get(&vs_define_output, dll, "vs_define_output", dname, me)) return -2;
  if (vss_get(&vs_define_parameter, dll, "vs_define_parameter", dname, me)) return -2;
  if (vss_get(&vs_define_units, dll, "vs_define_units", dname, me)) return -2;
  if (vss_get(&vs_define_variable, dll, "vs_define_variable", dname, me)) return -2;
  if (vss_get(&vs_get_var_ptr, dll, "vs_get_var_ptr", dname, me)) return -2;
  if (vss_get(&vs_get_var_ptr_int, dll, "vs_get_var_ptr_int", dname, me)) return -2;
  if (vss_get(&vs_set_units, dll, "vs_set_units", dname, me)) return -2;
  if (vss_get(&vs_install_calc_func, dll, "vs_install_calc_func", dname, me)) return -2;
  if (vss_get(&vs_printf, dll, "vs_printf", dname, me)) return -2;
  if (vss_get(&vs_printf_error, dll, "vs_printf_error", dname, me)) return -2;
  if (vss_get(&vs_set_sym_int, dll, "vs_set_sym_int", dname, me)) return -2;
  if (vss_get(&vs_set_sym_real, dll, "vs_set_sym_real", dname, me)) return -2;
  if (vss_get(&vs_set_sym_attribute, dll, "vs_set_sym_attribute", dname, me)) return -2;
  if (vss_get(&vs_read_next_line, dll, "vs_read_next_line", dname, me)) return -2;
  if (vss_get(&vs_write_to_echo_file, dll, "vs_write_to_echo_file", dname, me)) return -2;
  if (vss_get(&vs_write_header_to_echo_file, dll, "vs_write_header_to_echo_file", dname, me)) return -2;
  if (vss_get(&vs_write_f_to_echo_file, dll, "vs_write_f_to_echo_file", dname, me)) return -2;
  if (vss_get(&vs_write_i_to_echo_file, dll, "vs_write_i_to_echo_file", dname, me)) return -2;
  if (vss_get(&vs_get_sym_attribute, dll, "vs_get_sym_attribute", dname, me)) return -2;
  if (vss_get(&vs_define_parameter_int, dll, "vs_define_parameter_int", dname, me)) return -2;

  return 0;
}

int vs_get_api_road (HMODULE dll, char *dname)
{
  char *me = "vs_get_api_road";
  if (dll == NULL) return vss_print_no_dll(me, dname);

  if (vss_get(&vs_road_s, dll, "vs_road_s", dname, me)) return -2;
  if (vss_get(&vs_road_l, dll, "vs_road_l", dname, me)) return -2;
  if (vss_get(&vs_road_x, dll, "vs_road_x", dname, me)) return -2;
  if (vss_get(&vs_road_y, dll, "vs_road_y", dname, me)) return -2;
  if (vss_get(&vs_road_z, dll, "vs_road_z", dname, me)) return -2;
  if (vss_get(&vs_road_yaw, dll, "vs_road_yaw", dname, me)) return -2;
  if (vss_get(&vs_s_loop, dll, "vs_s_loop", dname, me)) return -2;
  if (vss_get(&vs_get_dzds_dzdl, dll, "vs_get_dzds_dzdl", dname, me)) return -2;
  if (vss_get(&vs_get_road_start_stop, dll, "vs_get_road_start_stop", dname, me)) return -2;
  if (vss_get(&vs_get_road_xyz, dll, "vs_get_road_xyz", dname, me)) return -2;
  if (vss_get(&vs_get_road_contact, dll, "vs_get_road_contact", dname, me)) return -2;
  if (vss_get(&vs_target_l, dll, "vs_target_l", dname, me)) return -2;
  if (vss_get(&vs_get_dzds_dzdl_i, dll, "vs_get_dzds_dzdl_i", dname, me)) return -2;
  if (vss_get(&vs_get_road_contact_sl, dll, "vs_get_road_contact_sl", dname, me)) return -2;
  if (vss_get(&vs_road_curv_i, dll, "vs_road_curv_i", dname, me)) return -2;
  if (vss_get(&vs_road_l_i, dll, "vs_road_l_i", dname, me)) return -2;
  if (vss_get(&vs_road_pitch_sl_i, dll, "vs_road_pitch_sl_i", dname, me)) return -2;
  if (vss_get(&vs_road_roll_sl_i, dll, "vs_road_roll_sl_i", dname, me)) return -2;
  if (vss_get(&vs_road_s_i, dll, "vs_road_s_i", dname, me)) return -2;
  if (vss_get(&vs_road_x_i, dll, "vs_road_x_i", dname, me)) return -2;
  if (vss_get(&vs_road_y_i, dll, "vs_road_y_i", dname, me)) return -2;
  if (vss_get(&vs_road_yaw_i, dll, "vs_road_yaw_i", dname, me)) return -2;
  if (vss_get(&vs_road_z_i, dll, "vs_road_z_i", dname, me)) return -2;
  if (vss_get(&vs_road_z_sl_i, dll, "vs_road_z_sl_i", dname, me)) return -2;

  return 0;
}

int vs_get_api_install_external (HMODULE dll, char *dname)
{
  char *me = "vs_get_api_install_external";
  if (dll == NULL) return vss_print_no_dll(me, dname);

  if (vss_get(&vs_run, dll, "vs_run", dname, me)) return -2;

  if (vss_get(&vs_install_calc_function, dll, "vs_install_calc_function", dname, me)) return -2;
  if (vss_get(&vs_install_echo_function, dll, "vs_install_echo_function", dname, me)) return -2;
  if (vss_get(&vs_install_setdef_function, dll, "vs_install_setdef_function", dname, me)) return -2;
  if (vss_get(&vs_install_scan_function, dll, "vs_install_scan_function", dname, me)) return -2;
  if (vss_get(&vs_install_free_function, dll, "vs_install_free_function", dname, me)) return -2;

  return 0;
}