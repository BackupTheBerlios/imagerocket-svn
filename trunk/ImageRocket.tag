<?xml version='1.0' encoding='ISO-8859-1' standalone='yes'?>
<tagfile>
  <compound kind="class">
    <name>RocketFilePreviewArea</name>
    <filename>classRocketFilePreviewArea.html</filename>
    <member kind="signal">
      <type>void</type>
      <name>clicked</name>
      <anchor>l0</anchor>
      <arglist>(int index)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>RocketFilePreviewArea</name>
      <anchor>a0</anchor>
      <arglist>(QWidget *parent, int thumbnailSize, RocketImageList *list)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>removeEntry</name>
      <anchor>a2</anchor>
      <arglist>(int index)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>clearEntries</name>
      <anchor>a3</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setActive</name>
      <anchor>a4</anchor>
      <arglist>(int index)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>getThumbnailSize</name>
      <anchor>a5</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="slot" protection="protected">
      <type>void</type>
      <name>clickedEvent</name>
      <anchor>j0</anchor>
      <arglist>(QWidget *w)</arglist>
    </member>
    <member kind="slot" protection="protected">
      <type>void</type>
      <name>listChanged</name>
      <anchor>j1</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="slot" protection="protected">
      <type>void</type>
      <name>updateSize</name>
      <anchor>j2</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function" protection="protected">
      <type>void</type>
      <name>resizeEvent</name>
      <anchor>b0</anchor>
      <arglist>(QResizeEvent *event)</arglist>
    </member>
    <member kind="function" protection="protected">
      <type>void</type>
      <name>centerOnPosition</name>
      <anchor>b1</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>QWidget *</type>
      <name>widget</name>
      <anchor>p0</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>RocketImageList *</type>
      <name>images</name>
      <anchor>p1</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>QVector&lt; RocketFilePreviewWidget * &gt;</type>
      <name>previews</name>
      <anchor>p2</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>int</type>
      <name>index</name>
      <anchor>p3</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>int</type>
      <name>thumbnailSize</name>
      <anchor>p4</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>RocketImageList</name>
    <filename>classRocketImageList.html</filename>
    <member kind="slot">
      <type>void</type>
      <name>updateThumbnail</name>
      <anchor>i0</anchor>
      <arglist>(const QString fileName, const QImage thumbnail)</arglist>
    </member>
    <member kind="signal">
      <type>void</type>
      <name>indexChanged</name>
      <anchor>l0</anchor>
      <arglist>(int index)</arglist>
    </member>
    <member kind="signal">
      <type>void</type>
      <name>listChanged</name>
      <anchor>l1</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="signal">
      <type>void</type>
      <name>thumbnailChanged</name>
      <anchor>l2</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setLocation</name>
      <anchor>a2</anchor>
      <arglist>(QString location)</arglist>
    </member>
    <member kind="function">
      <type>QString</type>
      <name>getLocation</name>
      <anchor>a3</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>unsigned int</type>
      <name>size</name>
      <anchor>a4</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>const QVector&lt; RocketImage * &gt; *</type>
      <name>getVector</name>
      <anchor>a5</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>QString</type>
      <name>getAsString</name>
      <anchor>a6</anchor>
      <arglist>(int index)</arglist>
    </member>
    <member kind="function">
      <type>QPixmap</type>
      <name>getThumbnail</name>
      <anchor>a7</anchor>
      <arglist>(int index)</arglist>
    </member>
    <member kind="function">
      <type>RocketImage *</type>
      <name>getAsRocketImage</name>
      <anchor>a8</anchor>
      <arglist>(int index)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setIndex</name>
      <anchor>a9</anchor>
      <arglist>(int index)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>getIndex</name>
      <anchor>a10</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function" protection="protected">
      <type>void</type>
      <name>continueThumbnailGeneration</name>
      <anchor>b0</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>QString</type>
      <name>location</name>
      <anchor>p0</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>int</type>
      <name>index</name>
      <anchor>p1</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>QVector&lt; RocketImage * &gt;</type>
      <name>list</name>
      <anchor>p2</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>ThreadedThumbnailGenerator *</type>
      <name>generator</name>
      <anchor>p3</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>RocketImageSquareContainer</name>
    <filename>classRocketImageSquareContainer.html</filename>
    <member kind="signal">
      <type>void</type>
      <name>zoomChanged</name>
      <anchor>l0</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>RocketImageSquareContainer</name>
      <anchor>a0</anchor>
      <arglist>(QPixmap *source, bool hasTransparency, int pieceSize)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setCached</name>
      <anchor>a2</anchor>
      <arglist>(int x, int y, bool newState)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>getPieceCount</name>
      <anchor>a3</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>QSize</type>
      <name>getSize</name>
      <anchor>a4</anchor>
      <arglist>(int x, int y)</arglist>
    </member>
    <member kind="function">
      <type>QPixmap *</type>
      <name>getPiece</name>
      <anchor>a5</anchor>
      <arglist>(int x, int y)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>getGridWidth</name>
      <anchor>a6</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>getGridHeight</name>
      <anchor>a7</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>QSize</type>
      <name>getGridSize</name>
      <anchor>a8</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>getScaledWidth</name>
      <anchor>a9</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>getScaledHeight</name>
      <anchor>a10</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>getPieceSize</name>
      <anchor>a11</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>getZoom</name>
      <anchor>a12</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setZoom</name>
      <anchor>a13</anchor>
      <arglist>(double zoom)</arglist>
    </member>
    <member kind="function" protection="protected">
      <type>int</type>
      <name>getIndex</name>
      <anchor>b0</anchor>
      <arglist>(int x, int y)</arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>QVector&lt; QPixmap * &gt;</type>
      <name>pieces</name>
      <anchor>p0</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>QPixmap *</type>
      <name>source</name>
      <anchor>p1</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>QPixmap *</type>
      <name>transparent</name>
      <anchor>p2</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>int</type>
      <name>pieceSize</name>
      <anchor>p3</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>int</type>
      <name>pieceCount</name>
      <anchor>p4</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>int</type>
      <name>scaledH</name>
      <anchor>p5</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>int</type>
      <name>scaledW</name>
      <anchor>p6</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>bool</type>
      <name>hasTransparency</name>
      <anchor>p7</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>double</type>
      <name>zoom</name>
      <anchor>p8</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>RocketView</name>
    <filename>classRocketView.html</filename>
    <member kind="signal">
      <type>void</type>
      <name>zoomChanged</name>
      <anchor>l0</anchor>
      <arglist>(double zoom)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>RocketView</name>
      <anchor>a0</anchor>
      <arglist>(QWidget *parent, int pieceSize)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>load</name>
      <anchor>a2</anchor>
      <arglist>(QString fileName)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>load</name>
      <anchor>a3</anchor>
      <arglist>(QImage newImage)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>load</name>
      <anchor>a4</anchor>
      <arglist>(QPixmap newPixmap, bool hasTransparency)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>resetToBlank</name>
      <anchor>a5</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>QPixmap *</type>
      <name>getPixmap</name>
      <anchor>a6</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>QSize</type>
      <name>imageSize</name>
      <anchor>a7</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>getZoom</name>
      <anchor>a8</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setZoom</name>
      <anchor>a9</anchor>
      <arglist>(double zoomFactor, int x=-1, int y=-1)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setZoom</name>
      <anchor>a10</anchor>
      <arglist>(double zoomFactor, QPoint zoomCenter)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>resetZoomAndPosition</name>
      <anchor>a11</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setFitToWidget</name>
      <anchor>a12</anchor>
      <arglist>(bool)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>willFitToWidget</name>
      <anchor>a13</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>center</name>
      <anchor>a14</anchor>
      <arglist>(int x, int y)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>center</name>
      <anchor>a15</anchor>
      <arglist>(QPoint point)</arglist>
    </member>
    <member kind="function">
      <type>QPoint</type>
      <name>visibleCenter</name>
      <anchor>a16</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>isNullImage</name>
      <anchor>a17</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="slot" protection="protected">
      <type>void</type>
      <name>scrollingTestTimeout</name>
      <anchor>j0</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="slot" protection="protected">
      <type>void</type>
      <name>preloaderTimeout</name>
      <anchor>j1</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function" protection="protected">
      <type>void</type>
      <name>resizeContents</name>
      <anchor>b0</anchor>
      <arglist>(int w, int h)</arglist>
    </member>
    <member kind="function" protection="protected">
      <type>void</type>
      <name>updateZoomForSize</name>
      <anchor>b1</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function" protection="protected">
      <type>void</type>
      <name>resizeEvent</name>
      <anchor>b2</anchor>
      <arglist>(QResizeEvent *e)</arglist>
    </member>
    <member kind="function" protection="protected">
      <type>void</type>
      <name>mousePressEvent</name>
      <anchor>b3</anchor>
      <arglist>(QMouseEvent *e)</arglist>
    </member>
    <member kind="function" protection="protected">
      <type>void</type>
      <name>mouseReleaseEvent</name>
      <anchor>b4</anchor>
      <arglist>(QMouseEvent *e)</arglist>
    </member>
    <member kind="function" protection="protected">
      <type>void</type>
      <name>mouseMoveEvent</name>
      <anchor>b5</anchor>
      <arglist>(QMouseEvent *e)</arglist>
    </member>
    <member kind="function" protection="protected">
      <type>void</type>
      <name>paintEvent</name>
      <anchor>b6</anchor>
      <arglist>(QPaintEvent *e)</arglist>
    </member>
    <member kind="function" protection="protected">
      <type>void</type>
      <name>createBorders</name>
      <anchor>b7</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>QPixmap *</type>
      <name>pix</name>
      <anchor>p0</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>QPixmap</type>
      <name>horizontalBorder</name>
      <anchor>p1</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>QPixmap</type>
      <name>verticalBorder</name>
      <anchor>p2</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>QPixmap</type>
      <name>nwCorner</name>
      <anchor>p3</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>QPixmap</type>
      <name>neCorner</name>
      <anchor>p4</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>QPixmap</type>
      <name>swCorner</name>
      <anchor>p5</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>QPixmap</type>
      <name>seCorner</name>
      <anchor>p6</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>bool</type>
      <name>transparency</name>
      <anchor>p7</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>bool</type>
      <name>fitToWidget</name>
      <anchor>p8</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>bool</type>
      <name>blockDrawing</name>
      <anchor>p9</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>bool</type>
      <name>brokenImage</name>
      <anchor>p10</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>RocketImageSquareContainer *</type>
      <name>squares</name>
      <anchor>p11</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>double</type>
      <name>zoom</name>
      <anchor>p12</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>int</type>
      <name>pieceSize</name>
      <anchor>p13</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>bool</type>
      <name>inResizeEvent</name>
      <anchor>p14</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>int</type>
      <name>middleButtonScrollX</name>
      <anchor>p15</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>int</type>
      <name>middleButtonScrollY</name>
      <anchor>p16</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>QTimer *</type>
      <name>preloader</name>
      <anchor>p17</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>QVector&lt; QPoint &gt;</type>
      <name>preloadPoints</name>
      <anchor>p18</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>QSize</type>
      <name>preloadSize</name>
      <anchor>p19</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>bool</type>
      <name>scrollingDown</name>
      <anchor>p20</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>QTimer *</type>
      <name>scrollingTest</name>
      <anchor>p21</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>RocketWindow</name>
    <filename>classRocketWindow.html</filename>
    <member kind="slot">
      <type>void</type>
      <name>addToPalette</name>
      <anchor>i0</anchor>
      <arglist>(QString name, QIcon &amp;icon)</arglist>
    </member>
    <member kind="slot">
      <type>void</type>
      <name>previewClicked</name>
      <anchor>i1</anchor>
      <arglist>(int index)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>RocketWindow</name>
      <anchor>a0</anchor>
      <arglist>(lua_State *L)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setDirectory</name>
      <anchor>a2</anchor>
      <arglist>(QString dirName)</arglist>
    </member>
    <member kind="slot" protection="protected">
      <type>void</type>
      <name>initGUI</name>
      <anchor>j0</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="slot" protection="protected">
      <type>void</type>
      <name>initObject</name>
      <anchor>j1</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="slot" protection="protected">
      <type>void</type>
      <name>openFolderClicked</name>
      <anchor>j2</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="slot" protection="protected">
      <type>void</type>
      <name>exitClicked</name>
      <anchor>j3</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="slot" protection="protected">
      <type>void</type>
      <name>firstClicked</name>
      <anchor>j4</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="slot" protection="protected">
      <type>void</type>
      <name>backClicked</name>
      <anchor>j5</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="slot" protection="protected">
      <type>void</type>
      <name>forwardClicked</name>
      <anchor>j6</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="slot" protection="protected">
      <type>void</type>
      <name>lastClicked</name>
      <anchor>j7</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="slot" protection="protected">
      <type>void</type>
      <name>undoClicked</name>
      <anchor>j8</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="slot" protection="protected">
      <type>void</type>
      <name>redoClicked</name>
      <anchor>j9</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="slot" protection="protected">
      <type>void</type>
      <name>zoomInClicked</name>
      <anchor>j10</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="slot" protection="protected">
      <type>void</type>
      <name>zoom100Clicked</name>
      <anchor>j11</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="slot" protection="protected">
      <type>void</type>
      <name>zoomOutClicked</name>
      <anchor>j12</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="slot" protection="protected">
      <type>void</type>
      <name>zoomFitToggled</name>
      <anchor>j13</anchor>
      <arglist>(bool)</arglist>
    </member>
    <member kind="slot" protection="protected">
      <type>void</type>
      <name>aboutClicked</name>
      <anchor>j14</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="slot" protection="protected">
      <type>void</type>
      <name>toolClicked</name>
      <anchor>j15</anchor>
      <arglist>(QListWidgetItem *item)</arglist>
    </member>
    <member kind="slot" protection="protected">
      <type>void</type>
      <name>updateGui</name>
      <anchor>j16</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function" protection="protected" virtualness="virtual">
      <type>virtual void</type>
      <name>setZoom</name>
      <anchor>b0</anchor>
      <arglist>(double zoom)</arglist>
    </member>
    <member kind="function" protection="protected" virtualness="virtual">
      <type>virtual void</type>
      <name>setIndex</name>
      <anchor>b1</anchor>
      <arglist>(unsigned int index)</arglist>
    </member>
    <member kind="function" protection="protected">
      <type>void</type>
      <name>closeEvent</name>
      <anchor>b2</anchor>
      <arglist>(QCloseEvent *e)</arglist>
    </member>
    <member kind="function" protection="protected">
      <type>void</type>
      <name>loadPlugins</name>
      <anchor>b3</anchor>
      <arglist>(QString dirPath)</arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>RocketView *</type>
      <name>view</name>
      <anchor>p0</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>RocketImageList</type>
      <name>images</name>
      <anchor>p1</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>lua_State *</type>
      <name>L</name>
      <anchor>p2</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>QList&lt; QObject * &gt;</type>
      <name>plugins</name>
      <anchor>p3</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>unsigned int</type>
      <name>index</name>
      <anchor>p4</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>QLabel *</type>
      <name>statusFile</name>
      <anchor>p5</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>QLabel *</type>
      <name>statusZoom</name>
      <anchor>p6</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>QLabel *</type>
      <name>statusSize</name>
      <anchor>p7</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>QMenu *</type>
      <name>mFile</name>
      <anchor>p8</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>QMenu *</type>
      <name>mEdit</name>
      <anchor>p9</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>QMenu *</type>
      <name>mView</name>
      <anchor>p10</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>QMenu *</type>
      <name>mHelp</name>
      <anchor>p11</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>QAction *</type>
      <name>aOpenFolder</name>
      <anchor>p12</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>QAction *</type>
      <name>aExit</name>
      <anchor>p13</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>QAction *</type>
      <name>aFirst</name>
      <anchor>p14</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>QAction *</type>
      <name>aBack</name>
      <anchor>p15</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>QAction *</type>
      <name>aForward</name>
      <anchor>p16</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>QAction *</type>
      <name>aLast</name>
      <anchor>p17</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>QAction *</type>
      <name>aZoomOut</name>
      <anchor>p18</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>QAction *</type>
      <name>aZoom100</name>
      <anchor>p19</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>QAction *</type>
      <name>aZoomIn</name>
      <anchor>p20</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>QAction *</type>
      <name>aZoomFit</name>
      <anchor>p21</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>QAction *</type>
      <name>aUndo</name>
      <anchor>p22</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>QAction *</type>
      <name>aRedo</name>
      <anchor>p23</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>QAction *</type>
      <name>aAbout</name>
      <anchor>p24</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>QDockWidget *</type>
      <name>dPalette</name>
      <anchor>p25</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>QDockWidget *</type>
      <name>dFiles</name>
      <anchor>p26</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>RocketToolBox *</type>
      <name>toolbox</name>
      <anchor>p27</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>QStringList</type>
      <name>imageNameFilters</name>
      <anchor>p28</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>QString</type>
      <name>lastDir</name>
      <anchor>p29</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>RocketFilePreviewArea *</type>
      <name>filePreviewArea</name>
      <anchor>p30</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="dir">
    <name>source/ImageRocket/app/</name>
    <path>/home/family/source/ImageRocket/app/</path>
    <filename>dir_000002.html</filename>
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
    <file>ThreadedThumbnailGenerator.cpp</file>
    <file>ThreadedThumbnailGenerator.h</file>
  </compound>
  <compound kind="dir">
    <name>source/ImageRocket/</name>
    <path>/home/family/source/ImageRocket/</path>
    <filename>dir_000001.html</filename>
    <dir>source/ImageRocket/app/</dir>
    <dir>source/ImageRocket/lua/</dir>
    <dir>source/ImageRocket/plugins/</dir>
  </compound>
  <compound kind="dir">
    <name>source/ImageRocket/lua/</name>
    <path>/home/family/source/ImageRocket/lua/</path>
    <filename>dir_000007.html</filename>
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
    <filename>dir_000003.html</filename>
    <file>rocket-16.xpm</file>
    <file>rocket-24.xpm</file>
  </compound>
  <compound kind="dir">
    <name>source/ImageRocket/plugins/</name>
    <path>/home/family/source/ImageRocket/plugins/</path>
    <filename>dir_000004.html</filename>
    <dir>source/ImageRocket/plugins/test/</dir>
  </compound>
  <compound kind="dir">
    <name>source/</name>
    <path>/home/family/source/</path>
    <filename>dir_000000.html</filename>
    <dir>source/ImageRocket/</dir>
  </compound>
  <compound kind="dir">
    <name>source/ImageRocket/plugins/test/</name>
    <path>/home/family/source/ImageRocket/plugins/test/</path>
    <filename>dir_000005.html</filename>
    <file>moc_test.cpp</file>
    <file>test.cpp</file>
    <file>test.h</file>
  </compound>
  <compound kind="dir">
    <name>source/ImageRocket/app/tmp/</name>
    <path>/home/family/source/ImageRocket/app/tmp/</path>
    <filename>dir_000006.html</filename>
    <file>moc_RocketAboutDialog.cpp</file>
    <file>moc_RocketFilePreviewArea.cpp</file>
    <file>moc_RocketFilePreviewWidget.cpp</file>
    <file>moc_RocketImage.cpp</file>
    <file>moc_RocketImageList.cpp</file>
    <file>moc_RocketImageSquareContainer.cpp</file>
    <file>moc_RocketToolBox.cpp</file>
    <file>moc_RocketView.cpp</file>
    <file>moc_RocketWindow.cpp</file>
    <file>moc_ThreadedThumbnailGenerator.cpp</file>
    <file>qrc_app.cpp</file>
    <file>qrc_pixmaps.cpp</file>
    <file>ui_aboutdialog.h</file>
  </compound>
</tagfile>
