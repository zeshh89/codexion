# codexion
codexion proyect for 42cursus common core


./codexion 100 300 50 50 50 1 50 edf \
| awk '/is compiling/{c++;gsub(/is compiling/,"\033[1;31m&\033[0m")}1;END{print "\n\033[1;32mTotal: "c"\033[0m"}'

valgrind --tool=helgrind
