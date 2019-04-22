%~dp0glslangValidator -V %~dp0shader.vert -o %~dp0vert.spv
%~dp0glslangValidator -V %~dp0shader.frag -o %~dp0frag.spv
%~dp0glslangValidator -V %~dp0rt_shader.rgen -o %~dp0rgen.spv
%~dp0glslangValidator -V %~dp0rt_shader.rchit -o %~dp0rchit.spv
%~dp0glslangValidator -V %~dp0rt_shader.rmiss -o %~dp0rmiss.spv
pause

