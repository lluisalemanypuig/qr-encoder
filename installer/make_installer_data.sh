#!/bin/bash

CONFIG_DIR=installer/qr-encoder/config
DATA_DIR=installer/qr-encoder/packages/com.vendor.product/data
META_DIR=installer/qr-encoder/packages/com.vendor.product/meta
VERSION_STR="yy.mm"

mkdir -p $CONFIG_DIR
mkdir -p $DATA_DIR
mkdir -p $META_DIR

# copy execuatble
cp build-release/qr-encoder $DATA_DIR
# copy translations file
cp build-release/qmake_qmake_qm_files.qrc $DATA_DIR
# copy LICENSE file (GNU Affero GPL)
cp LICENSE $META_DIR

# copy source code
zip -r $DATA_DIR/source_code.zip src/

# write package.xml file
echo "<?xml version=\"1.0\" encoding=\"UTF-8\"?>
<Package>
    <DisplayName>QR Encoder</DisplayName>
    <Description>This will install QR Encoder.</Description>
    <Version>0.1.0-1</Version>
    <ReleaseDate>$(date +"%Y/%m/%d")</ReleaseDate>
    <Licenses>
        <License name=\"GNU Affero GPL\" file=\"LICENSE\" />
    </Licenses>
</Package>
" > $META_DIR/package.xml

echo "<?xml version=\"1.0\" encoding=\"UTF-8\"?>
<Installer>
    <Name>QR Encoder</Name>
    <Version>$VERSION_STR</Version>
    <Title>Installer of QR Encoder</Title>
    <Publisher>Lluís Alemany Puig</Publisher>
    <StartMenuDir>Desktop</StartMenuDir>
    <TargetDir>@HomeDir@/QR-Encoder</TargetDir>
</Installer>
" > $CONFIG_DIR/config.xml


# run binary creator from Qt Installer Framework
cd installer/qr-encoder
~/Qt/Tools/QtInstallerFramework/4.7/bin/binarycreator -c config/config.xml -p packages QR-Encoder-Installer
cd ../..

mv installer/qr-encoder/QR-Encoder-Installer .
