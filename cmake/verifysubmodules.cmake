#################################################################################
#
# Inviwo - Interactive Visualization Workshop
#
# Copyright (c) 2013-2018 Inviwo Foundation
# All rights reserved.
# 
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met: 
# 
# 1. Redistributions of source code must retain the above copyright notice, this
# list of conditions and the following disclaimer. 
# 2. Redistributions in binary form must reproduce the above copyright notice,
# this list of conditions and the following disclaimer in the documentation
# and/or other materials provided with the distribution. 
# 
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
# ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
# (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
# ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
# SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
# 
#################################################################################

#--------------------------------------------------------------------
# Verify that all submodules are present
function(verify_submodules modules_file)
	get_filename_component(dir ${modules_file} DIRECTORY)
	file(READ ${modules_file} contents)
    string(REPLACE "\n" ";" lines "${contents}")
    foreach(line ${lines})
        #^\s*path\s*=\s*(\S+)$*
        string(REGEX MATCH "^[ \t]*path[ \t]*=[ \t]*([^ \t]*)[ \t]*$" found_item ${line})
        if(CMAKE_MATCH_1)
			file(GLOB dircontents RELATIVE ${dir}/${CMAKE_MATCH_1} ${dir}/${CMAKE_MATCH_1}/[^.]*)
			list(LENGTH dircontents len)
			if(${len} EQUAL 0)
				message(FATAL_ERROR "Inviwo found an emtpy submodule at '${CMAKE_MATCH_1}'. "
					"This probably means that you have not cloned all submodules. Run \n" 
					"git submodule update --init\nto clone them.")
			endif()
		endif()
     endforeach()
endfunction()