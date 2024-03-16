#!/bin/bash

REPLACE_WITH="24.03"

sed -i 's/yy.mm/'$REPLACE_WITH'/g' make_installer_data.sh
sed -i 's/yy.mm/'$REPLACE_WITH'/g' src/mainwindow.cpp

sed -i 's/yy.mm/'$REPLACE_WITH'/g' src/qr-encoder_ca_ES.ts
sed -i 's/yy.mm/'$REPLACE_WITH'/g' src/qr-encoder_en_GB.ts
sed -i 's/yy.mm/'$REPLACE_WITH'/g' src/qr-encoder_es_ES.ts
