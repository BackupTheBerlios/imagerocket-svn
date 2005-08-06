<?xml version='1.0' encoding='ISO-8859-1' standalone='yes' ?>
<tagfile>
  <compound kind="class">
    <name>RocketImageSquareContainer</name>
    <filename>classRocketImageSquareContainer.htm</filename>
    <member kind="signal">
      <type>void</type>
      <name>zoomChanged</name>
      <anchorfile>classRocketImageSquareContainer.htm</anchorfile>
      <anchor>l0</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>RocketImageSquareContainer</name>
      <anchorfile>classRocketImageSquareContainer.htm</anchorfile>
      <anchor>a0</anchor>
      <arglist>(QPixmap *source, bool hasTransparency, int pieceSize)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setCached</name>
      <anchorfile>classRocketImageSquareContainer.htm</anchorfile>
      <anchor>a2</anchor>
      <arglist>(int x, int y, bool newState)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>getPieceCount</name>
      <anchorfile>classRocketImageSquareContainer.htm</anchorfile>
      <anchor>a3</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>QSize</type>
      <name>getSize</name>
      <anchorfile>classRocketImageSquareContainer.htm</anchorfile>
      <anchor>a4</anchor>
      <arglist>(int x, int y)</arglist>
    </member>
    <member kind="function">
      <type>QPixmap *</type>
      <name>getPiece</name>
      <anchorfile>classRocketImageSquareContainer.htm</anchorfile>
      <anchor>a5</anchor>
      <arglist>(int x, int y)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>getGridWidth</name>
      <anchorfile>classRocketImageSquareContainer.htm</anchorfile>
      <anchor>a6</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>getGridHeight</name>
      <anchorfile>classRocketImageSquareContainer.htm</anchorfile>
      <anchor>a7</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>QSize</type>
      <name>getGridSize</name>
      <anchorfile>classRocketImageSquareContainer.htm</anchorfile>
      <anchor>a8</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>getScaledWidth</name>
      <anchorfile>classRocketImageSquareContainer.htm</anchorfile>
      <anchor>a9</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>getScaledHeight</name>
      <anchorfile>classRocketImageSquareContainer.htm</anchorfile>
      <anchor>a10</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>getPieceSize</name>
      <anchorfile>classRocketImageSquareContainer.htm</anchorfile>
      <anchor>a11</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>getZoom</name>
      <anchorfile>classRocketImageSquareContainer.htm</anchorfile>
      <anchor>a12</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setZoom</name>
      <anchorfile>classRocketImageSquareContainer.htm</anchorfile>
      <anchor>a13</anchor>
      <arglist>(double zoom)</arglist>
    </member>
    <member kind="function" protection="protected">
      <type>int</type>
      <name>getIndex</name>
      <anchorfile>classRocketImageSquareContainer.htm</anchorfile>
      <anchor>b0</anchor>
      <arglist>(int x, int y)</arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>std::vector&lt; QPixmap * &gt;</type>
      <name>pieces</name>
      <anchorfile>classRocketImageSquareContainer.htm</anchorfile>
      <anchor>p0</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>QPixmap *</type>
      <name>source</name>
      <anchorfile>classRocketImageSquareContainer.htm</anchorfile>
      <anchor>p1</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>QPixmap *</type>
      <name>transparent</name>
      <anchorfile>classRocketImageSquareContainer.htm</anchorfile>
      <anchor>p2</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>int</type>
      <name>pieceSize</name>
      <anchorfile>classRocketImageSquareContainer.htm</anchorfile>
      <anchor>p3</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>int</type>
      <name>pieceCount</name>
      <anchorfile>classRocketImageSquareContainer.htm</anchorfile>
      <anchor>p4</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>int</type>
      <name>scaledH</name>
      <anchorfile>classRocketImageSquareContainer.htm</anchorfile>
      <anchor>p5</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>int</type>
      <name>scaledW</name>
      <anchorfile>classRocketImageSquareContainer.htm</anchorfile>
      <anchor>p6</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>bool</type>
      <name>hasTransparency</name>
      <anchorfile>classRocketImageSquareContainer.htm</anchorfile>
      <anchor>p7</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>double</type>
      <name>zoom</name>
      <anchorfile>classRocketImageSquareContainer.htm</anchorfile>
      <anchor>p8</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>RocketView</name>
    <filename>classRocketView.htm</filename>
    <member kind="signal">
      <type>void</type>
      <name>zoomChanged</name>
      <anchorfile>classRocketView.htm</anchorfile>
      <anchor>l0</anchor>
      <arglist>(double zoom)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>RocketView</name>
      <anchorfile>classRocketView.htm</anchorfile>
      <anchor>a0</anchor>
      <arglist>(QWidget *parent, int pieceSize)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>load</name>
      <anchorfile>classRocketView.htm</anchorfile>
      <anchor>a2</anchor>
      <arglist>(QString fileName)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>load</name>
      <anchorfile>classRocketView.htm</anchorfile>
      <anchor>a3</anchor>
      <arglist>(QImage newImage)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>load</name>
      <anchorfile>classRocketView.htm</anchorfile>
      <anchor>a4</anchor>
      <arglist>(QPixmap newPixmap, bool hasTransparency)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>resetToBlank</name>
      <anchorfile>classRocketView.htm</anchorfile>
      <anchor>a5</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>QPixmap *</type>
      <name>getPixmap</name>
      <anchorfile>classRocketView.htm</anchorfile>
      <anchor>a6</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>QSize</type>
      <name>imageSize</name>
      <anchorfile>classRocketView.htm</anchorfile>
      <anchor>a7</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>getZoom</name>
      <anchorfile>classRocketView.htm</anchorfile>
      <anchor>a8</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setZoom</name>
      <anchorfile>classRocketView.htm</anchorfile>
      <anchor>a9</anchor>
      <arglist>(double zoomFactor, int x=-1, int y=-1)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setZoom</name>
      <anchorfile>classRocketView.htm</anchorfile>
      <anchor>a10</anchor>
      <arglist>(double zoomFactor, QPoint zoomCenter)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setFitToWidget</name>
      <anchorfile>classRocketView.htm</anchorfile>
      <anchor>a11</anchor>
      <arglist>(bool)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>willFitToWidget</name>
      <anchorfile>classRocketView.htm</anchorfile>
      <anchor>a12</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>center</name>
      <anchorfile>classRocketView.htm</anchorfile>
      <anchor>a13</anchor>
      <arglist>(int x, int y)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>center</name>
      <anchorfile>classRocketView.htm</anchorfile>
      <anchor>a14</anchor>
      <arglist>(QPoint point)</arglist>
    </member>
    <member kind="function">
      <type>QPoint</type>
      <name>visibleCenter</name>
      <anchorfile>classRocketView.htm</anchorfile>
      <anchor>a15</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>isNullImage</name>
      <anchorfile>classRocketView.htm</anchorfile>
      <anchor>a16</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="slot" protection="protected">
      <type>void</type>
      <name>scrollingTestTimeout</name>
      <anchorfile>classRocketView.htm</anchorfile>
      <anchor>j0</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="slot" protection="protected">
      <type>void</type>
      <name>preloaderTimeout</name>
      <anchorfile>classRocketView.htm</anchorfile>
      <anchor>j1</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function" protection="protected">
      <type>void</type>
      <name>resizeContents</name>
      <anchorfile>classRocketView.htm</anchorfile>
      <anchor>b0</anchor>
      <arglist>(int w, int h)</arglist>
    </member>
    <member kind="function" protection="protected">
      <type>void</type>
      <name>updateZoomForSize</name>
      <anchorfile>classRocketView.htm</anchorfile>
      <anchor>b1</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function" protection="protected">
      <type>void</type>
      <name>resizeEvent</name>
      <anchorfile>classRocketView.htm</anchorfile>
      <anchor>b2</anchor>
      <arglist>(QResizeEvent *e)</arglist>
    </member>
    <member kind="function" protection="protected">
      <type>void</type>
      <name>mousePressEvent</name>
      <anchorfile>classRocketView.htm</anchorfile>
      <anchor>b3</anchor>
      <arglist>(QMouseEvent *e)</arglist>
    </member>
    <member kind="function" protection="protected">
      <type>void</type>
      <name>mouseReleaseEvent</name>
      <anchorfile>classRocketView.htm</anchorfile>
      <anchor>b4</anchor>
      <arglist>(QMouseEvent *e)</arglist>
    </member>
    <member kind="function" protection="protected">
      <type>void</type>
      <name>mouseMoveEvent</name>
      <anchorfile>classRocketView.htm</anchorfile>
      <anchor>b5</anchor>
      <arglist>(QMouseEvent *e)</arglist>
    </member>
    <member kind="function" protection="protected">
      <type>void</type>
      <name>paintEvent</name>
      <anchorfile>classRocketView.htm</anchorfile>
      <anchor>b6</anchor>
      <arglist>(QPaintEvent *e)</arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>QPixmap *</type>
      <name>pix</name>
      <anchorfile>classRocketView.htm</anchorfile>
      <anchor>p0</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>QPixmap *</type>
      <name>horizontalBorder</name>
      <anchorfile>classRocketView.htm</anchorfile>
      <anchor>p1</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>QPixmap *</type>
      <name>verticalBorder</name>
      <anchorfile>classRocketView.htm</anchorfile>
      <anchor>p2</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>bool</type>
      <name>transparency</name>
      <anchorfile>classRocketView.htm</anchorfile>
      <anchor>p3</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>bool</type>
      <name>fitToWidget</name>
      <anchorfile>classRocketView.htm</anchorfile>
      <anchor>p4</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>bool</type>
      <name>blockDrawing</name>
      <anchorfile>classRocketView.htm</anchorfile>
      <anchor>p5</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>bool</type>
      <name>brokenImage</name>
      <anchorfile>classRocketView.htm</anchorfile>
      <anchor>p6</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>RocketImageSquareContainer *</type>
      <name>squares</name>
      <anchorfile>classRocketView.htm</anchorfile>
      <anchor>p7</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>double</type>
      <name>zoom</name>
      <anchorfile>classRocketView.htm</anchorfile>
      <anchor>p8</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>int</type>
      <name>pieceSize</name>
      <anchorfile>classRocketView.htm</anchorfile>
      <anchor>p9</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>bool</type>
      <name>inResizeEvent</name>
      <anchorfile>classRocketView.htm</anchorfile>
      <anchor>p10</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>int</type>
      <name>scrollX</name>
      <anchorfile>classRocketView.htm</anchorfile>
      <anchor>p11</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>int</type>
      <name>scrollY</name>
      <anchorfile>classRocketView.htm</anchorfile>
      <anchor>p12</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>QTimer *</type>
      <name>preloader</name>
      <anchorfile>classRocketView.htm</anchorfile>
      <anchor>p13</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>QVector&lt; QPoint &gt;</type>
      <name>preloadPoints</name>
      <anchorfile>classRocketView.htm</anchorfile>
      <anchor>p14</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>QSize</type>
      <name>preloadSize</name>
      <anchorfile>classRocketView.htm</anchorfile>
      <anchor>p15</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>bool</type>
      <name>scrollingDown</name>
      <anchorfile>classRocketView.htm</anchorfile>
      <anchor>p16</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>QTimer *</type>
      <name>scrollingTest</name>
      <anchorfile>classRocketView.htm</anchorfile>
      <anchor>p17</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="dir">
    <name>source/ImageRocket/app/</name>
    <path>/home/family/source/ImageRocket/app/</path>
    <filename>dir_000002.htm</filename>
    <dir>source/ImageRocket/app/pixmaps/</dir>
    <dir>source/ImageRocket/app/tmp/</dir>
    <file>consts.h</file>
    <file>interfaces.h</file>
    <file>main.cpp</file>
    <file>RocketAboutDialog.cpp</file>
    <file>RocketAboutDialog.h</file>
    <file>RocketFilePreviewArea.cpp</file>
    <file>RocketFilePreviewArea.h</file>
    <file>RocketFilePreviewWidget.cpp</file>
    <file>RocketFilePreviewWidget.h</file>
    <file>RocketImage.cpp</file>
    <file>RocketImage.h</file>
    <file>RocketImageList.cpp</file>
    <file>RocketImageList.h</file>
    <file>RocketImageSquareContainer.cpp</file>
    <file>RocketImageSquareContainer.h</file>
    <file>RocketToolBox.cpp</file>
    <file>RocketToolBox.h</file>
    <file>RocketView.cpp</file>
    <file>RocketView.h</file>
    <file>RocketWindow.cpp</file>
    <file>RocketWindow.h</file>
    <file>test.cpp</file>
    <file>test.h</file>
  </compound>
  <compound kind="dir">
    <name>source/ImageRocket/</name>
    <path>/home/family/source/ImageRocket/</path>
    <filename>dir_000001.htm</filename>
    <dir>source/ImageRocket/app/</dir>
    <dir>source/ImageRocket/lua/</dir>
    <dir>source/ImageRocket/plugins/</dir>
    <file>ui_aboutdialog.h</file>
  </compound>
  <compound kind="dir">
    <name>source/ImageRocket/lua/</name>
    <path>/home/family/source/ImageRocket/lua/</path>
    <filename>dir_000007.htm</filename>
    <file>lapi.c</file>
    <file>lapi.h</file>
    <file>lauxlib.c</file>
    <file>lauxlib.h</file>
    <file>lbaselib.c</file>
    <file>lcode.c</file>
    <file>lcode.h</file>
    <file>ldblib.c</file>
    <file>ldebug.c</file>
    <file>ldebug.h</file>
    <file>ldo.c</file>
    <file>ldo.h</file>
    <file>ldump.c</file>
    <file>lfunc.c</file>
    <file>lfunc.h</file>
    <file>lgc.c</file>
    <file>lgc.h</file>
    <file>linit.c</file>
    <file>liolib.c</file>
    <file>llex.c</file>
    <file>llex.h</file>
    <file>llimits.h</file>
    <file>lmathlib.c</file>
    <file>lmem.c</file>
    <file>lmem.h</file>
    <file>loadlib.c</file>
    <file>lobject.c</file>
    <file>lobject.h</file>
    <file>lopcodes.c</file>
    <file>lopcodes.h</file>
    <file>loslib.c</file>
    <file>lparser.c</file>
    <file>lparser.h</file>
    <file>lstate.c</file>
    <file>lstate.h</file>
    <file>lstring.c</file>
    <file>lstring.h</file>
    <file>lstrlib.c</file>
    <file>ltable.c</file>
    <file>ltable.h</file>
    <file>ltablib.c</file>
    <file>ltm.c</file>
    <file>ltm.h</file>
    <file>lua.h</file>
    <file>luaconf.h</file>
    <file>lualib.h</file>
    <file>lundump.c</file>
    <file>lundump.h</file>
    <file>lvm.c</file>
    <file>lvm.h</file>
    <file>lzio.c</file>
    <file>lzio.h</file>
    <file>print.c</file>
  </compound>
  <compound kind="dir">
    <name>source/ImageRocket/app/pixmaps/</name>
    <path>/home/family/source/ImageRocket/app/pixmaps/</path>
    <filename>dir_000003.htm</filename>
    <file>rocket-16.xpm</file>
    <file>rocket-24.xpm</file>
  </compound>
  <compound kind="dir">
    <name>source/ImageRocket/plugins/</name>
    <path>/home/family/source/ImageRocket/plugins/</path>
    <filename>dir_000004.htm</filename>
    <dir>source/ImageRocket/plugins/test/</dir>
  </compound>
  <compound kind="dir">
    <name>source/</name>
    <path>/home/family/source/</path>
    <filename>dir_000000.htm</filename>
    <dir>source/ImageRocket/</dir>
  </compound>
  <compound kind="dir">
    <name>source/ImageRocket/plugins/test/</name>
    <path>/home/family/source/ImageRocket/plugins/test/</path>
    <filename>dir_000005.htm</filename>
    <file>moc_test.cpp</file>
    <file>test.cpp</file>
    <file>test.h</file>
  </compound>
  <compound kind="dir">
    <name>source/ImageRocket/app/tmp/</name>
    <path>/home/family/source/ImageRocket/app/tmp/</path>
    <filename>dir_000006.htm</filename>
    <file>moc_RocketAboutDialog.cpp</file>
    <file>moc_RocketFilePreviewArea.cpp</file>
    <file>moc_RocketFilePreviewWidget.cpp</file>
    <file>moc_RocketImage.cpp</file>
    <file>moc_RocketImageCollection.cpp</file>
    <file>moc_RocketImageList.cpp</file>
    <file>moc_RocketImageSquareContainer.cpp</file>
    <file>moc_RocketToolBox.cpp</file>
    <file>moc_RocketView.cpp</file>
    <file>moc_RocketWindow.cpp</file>
    <file>qrc_app.cpp</file>
    <file>qrc_pixmaps.cpp</file>
    <file>ui_aboutdialog.h</file>
  </compound>
</tagfile>
