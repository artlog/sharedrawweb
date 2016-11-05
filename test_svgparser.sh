#!/bin/bash

echo '#!/bin/bash' >test_rect.sh
echo -n 'dist/bezier 10 "0 to 54" test_rect.ima '>test_rect.sh
EOF

make build/svgparser
build/svgparser ../cube_opengl/resources/explode1.svg >> test_rect.sh

chmod u+x test_rect.sh
./test_rect.sh
