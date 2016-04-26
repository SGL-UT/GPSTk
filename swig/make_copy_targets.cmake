#----------------------------------------
# gpstk/swig/make_copy_targets.cmake
#----------------------------------------

cmake_minimum_required( VERSION 2.8.10  )

function(make_copy_targets file_list source_dir dest_dir)
    foreach(fn ${${file_list}})
      set(sfn "${source_dir}/${fn}")
      set(dfn "${dest_dir}/${fn}")
      #message(STATUS "${fn} -> ${dest_dir}")
      add_custom_command(OUTPUT ${dfn} COMMAND cmake -E copy_if_different "${sfn}" "${dfn}" DEPENDS "${sfn}")
      list(APPEND targets "${dfn}")
    endforeach(fn)
    add_custom_target(${file_list}_target ALL DEPENDS ${targets}) 
endfunction(make_copy_targets)
