add_executable(compile_compute_shader compile_compute_shader.cc) 
target_link_libraries(compile_compute_shader PUBLIC MachineIndependent)
target_include_directories(compile_compute_shader PUBLIC ../glslang ${CMAKE_CURRENT_SOURCE_DIR})

add_library(
	lsp STATIC 
	protocol.cc
	protocol.hpp
	completion.cc
	completion.hpp
    semantic_token.cc
    semantic_token.hpp
	doc.cc
	doc.hpp
	workspace.cc
	workspace.hpp
	parser.cc
	parser.hpp
	document_symbol.cc
	lsp_defs.hpp
    args.cc
    args.hpp
    compute_inactive.hpp
    compute_inactive.cc
)

target_link_libraries(lsp PUBLIC MachineIndependent nlohmann_json::nlohmann_json)
target_include_directories(lsp PUBLIC ../json/include/ ../glslang ${CMAKE_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(glslx glslx.cc)
add_executable(test_protocol test_protocol.cc)
target_link_libraries(glslx PUBLIC lsp)
target_link_libraries(test_protocol PUBLIC lsp)

add_executable(test_options test_options.cc)
target_link_libraries(test_options lsp)
