echo "hpp2plantuml -i $(find . -name "*.hpp" | grep -v "sol" | grep -v "nlohmann" | tr '\n' ' ' | sed 's/ / -i /g') -o UML" | sed 's/ -i  -o / -o /g'  >generate_uml.sh
