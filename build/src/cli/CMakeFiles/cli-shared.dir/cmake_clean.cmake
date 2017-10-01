FILE(REMOVE_RECURSE
  "../../lib/libcli.pdb"
  "../../lib/libcli.so"
)

# Per-language clean rules from dependency scanning.
FOREACH(lang)
  INCLUDE(CMakeFiles/cli-shared.dir/cmake_clean_${lang}.cmake OPTIONAL)
ENDFOREACH(lang)
