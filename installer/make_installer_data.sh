#!/bin/bash

CONFIG_DIR=installer/qr-encoder/config
DATA_DIR=installer/qr-encoder/packages/com.vendor.product/data
META_DIR=installer/qr-encoder/packages/com.vendor.product/meta
VERSION_STR="yy.mm"

mkdir -p $CONFIG_DIR
mkdir -p $DATA_DIR
mkdir -p $META_DIR

# copy source code
zip -r $DATA_DIR/source_code.zip src/

# copy LICENSE file (GNU Affero GPL)
cp LICENSE $META_DIR

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

Qt_PATH=""

if [ "$OSTYPE" == "msys" ]; then
	
	# copy executable
	cp build/release/qr-encoder $DATA_DIR
	# copy translations file
	cp build/release/qmake_qmake_qm_files.qrc $DATA_DIR
	
	Qt_PATH=C:/Qt
	Qt_BIN_PATH=$Qt_PATH/6.6.1/mingw_64/bin
	Qt_PLUGIN_PATH=$Qt_PATH/6.6.1/mingw_64/plugins
	
	# copy Qt dll files
	cp $Qt_BIN_PATH/Qt6Widgets.dll $DATA_DIR
	cp $Qt_BIN_PATH/Qt6Gui.dll $DATA_DIR
	cp $Qt_BIN_PATH/Qt6Core.dll $DATA_DIR
	# copy system dlls and gcc's dlls
	cp $Qt_BIN_PATH/libgcc_s_seh-1.dll $DATA_DIR
	cp $Qt_BIN_PATH/libstdc++-6.dll $DATA_DIR
	cp $Qt_BIN_PATH/libwinpthread-1.dll $DATA_DIR
	
	# copy plugins
	mkdir -p $DATA_DIR/plugins/platforms
	cp $Qt_PLUGIN_PATH/platforms/qwindows.dll $DATA_DIR/plugins/platforms
	mkdir -p $DATA_DIR/plugins/styles
	cp $Qt_PLUGIN_PATH/styles/qwindowsvistastyle.dll $DATA_DIR/plugins/styles
	
elif [ "$OSTYPE" == "linux-gnu" ]; then
	
	# copy executable
	cp build-release/qr-encoder $DATA_DIR
	# copy translations file
	cp build-release/qmake_qmake_qm_files.qrc $DATA_DIR
	
	Qt_PATH=~/Qt
	
fi

cd installer/qr-encoder

# run binary creator from Qt Installer Framework
$Qt_PATH/Tools/QtInstallerFramework/4.7/bin/binarycreator -c config/config.xml -p packages QR-Encoder-Installer

cd ../..

mv installer/qr-encoder/QR-Encoder-Installer .
