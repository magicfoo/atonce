<map version="0.9.0">
<!-- To view this file, download free mind mapping software FreeMind from http://freemind.sourceforge.net -->
<node BACKGROUND_COLOR="#333333" COLOR="#ffff66" CREATED="1298448969806" ID="ID_1261975709" MODIFIED="1298555782086" STYLE="bubble">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p style="text-align: center">
      <b>jam message </b>
    </p>
    <p style="text-align: center">
      <b>protocoles</b>
    </p>
  </body>
</html></richcontent>
<edge WIDTH="2"/>
<font BOLD="true" NAME="SansSerif" SIZE="18"/>
<node BACKGROUND_COLOR="#333333" COLOR="#ccff00" CREATED="1298452381992" FOLDED="true" HGAP="72" ID="ID_558360663" MODIFIED="1309186398143" POSITION="right" STYLE="bubble" TEXT="jam.session-man" VSHIFT="-17">
<edge COLOR="#808080" STYLE="bezier" WIDTH="2"/>
<font BOLD="true" NAME="SansSerif" SIZE="12"/>
<node BACKGROUND_COLOR="#ffcc33" COLOR="#333333" CREATED="1298452527096" FOLDED="true" ID="ID_405465550" MODIFIED="1308043173519" STYLE="bubble" TEXT="open-session">
<font NAME="SansSerif" SIZE="12"/>
<node BACKGROUND_COLOR="#cccc00" COLOR="#333333" CREATED="1298453193472" FOLDED="true" HGAP="63" ID="ID_1918495691" MODIFIED="1298555633186" STYLE="bubble" TEXT="start a session" VSHIFT="-20">
<attribute_layout NAME_WIDTH="33" VALUE_WIDTH="67"/>
<attribute NAME="in" VALUE="name:string"/>
<node BACKGROUND_COLOR="#e8e8e8" COLOR="#333333" CREATED="1298498958799" ID="ID_460449262" MODIFIED="1298536368354" STYLE="bubble">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      <b>name</b>
    </p>
    <p>
      String name of the session to open
    </p>
  </body>
</html></richcontent>
</node>
<node BACKGROUND_COLOR="#ffffff" COLOR="#6666ff" CREATED="1298453207904" HGAP="42" ID="ID_188597473" LINK="#ID_1856756731" MODIFIED="1298536370424" STYLE="fork" TEXT="present-layout (name=&apos;last&apos;)"/>
</node>
</node>
<node BACKGROUND_COLOR="#ffcc33" COLOR="#333333" CREATED="1298452542128" FOLDED="true" ID="ID_1626118461" MODIFIED="1308043174010" STYLE="bubble" TEXT="select-session">
<font NAME="SansSerif" SIZE="12"/>
<node BACKGROUND_COLOR="#cccc00" COLOR="#333333" CREATED="1298453193472" FOLDED="true" HGAP="51" ID="ID_188557955" MODIFIED="1298555633187" STYLE="bubble" TEXT="change to another session" VSHIFT="12">
<attribute_layout NAME_WIDTH="33" VALUE_WIDTH="67"/>
<node BACKGROUND_COLOR="#e8e8e8" COLOR="#333333" CREATED="1298461281017" HGAP="62" ID="ID_1076778234" MODIFIED="1298502605377" STYLE="bubble" TEXT="Display a session chooser and active the selected one" VSHIFT="-1"/>
<node BACKGROUND_COLOR="#ffffff" COLOR="#6666ff" CREATED="1298453207904" HGAP="42" ID="ID_279836056" LINK="#ID_1856756731" MODIFIED="1298498635039" STYLE="fork" TEXT="present-layout (name=&apos;X&apos;)"/>
</node>
</node>
<node BACKGROUND_COLOR="#ffcc33" COLOR="#333333" CREATED="1298452553120" FOLDED="true" ID="ID_1748697580" MODIFIED="1308043174751" STYLE="bubble" TEXT="backup-session">
<font NAME="SansSerif" SIZE="12"/>
<node BACKGROUND_COLOR="#cccc00" COLOR="#333333" CREATED="1298453193472" HGAP="45" ID="ID_191941637" MODIFIED="1304695791939" STYLE="bubble" VSHIFT="21">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      full backup of the current session
    </p>
    <p>
      before closing (mostly)
    </p>
  </body>
</html></richcontent>
<node BACKGROUND_COLOR="#ffffff" COLOR="#6666ff" CREATED="1298453207904" HGAP="42" ID="ID_1113467862" LINK="#ID_643311812" MODIFIED="1298502635763" STYLE="fork" TEXT="backup-layout (for each layout)"/>
</node>
</node>
</node>
<node BACKGROUND_COLOR="#333333" COLOR="#ccff00" CREATED="1298452368416" HGAP="78" ID="ID_1852134963" MODIFIED="1309186399878" POSITION="right" STYLE="bubble" TEXT="jam.layout-man" VSHIFT="-9">
<edge COLOR="#808080" STYLE="bezier" WIDTH="2"/>
<font BOLD="true" NAME="SansSerif" SIZE="12"/>
<node BACKGROUND_COLOR="#ffcc33" COLOR="#333333" CREATED="1298452415096" FOLDED="true" ID="ID_1646471762" MODIFIED="1304695797514" STYLE="bubble" TEXT="inspect-layout">
<font NAME="SansSerif" SIZE="12"/>
<node BACKGROUND_COLOR="#cccc00" COLOR="#333333" CREATED="1298454250840" FOLDED="true" HGAP="33" ID="ID_686117392" MODIFIED="1298555782082" STYLE="bubble" VSHIFT="-1">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      inspect the configuration
    </p>
    <p>
      of a named layout
    </p>
  </body>
</html></richcontent>
<font NAME="SansSerif" SIZE="12"/>
<attribute_layout NAME_WIDTH="33" VALUE_WIDTH="144"/>
<attribute NAME="in" VALUE="name:string"/>
<attribute NAME="out" VALUE="configuration:array"/>
<node BACKGROUND_COLOR="#e8e8e8" COLOR="#333333" CREATED="1298454188160" HGAP="45" ID="ID_469601146" MODIFIED="1298544079953" STYLE="bubble" VSHIFT="8">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      <b>name </b>
    </p>
    <p>
      name of the layout to inspect
    </p>
    <p>
      
    </p>
    <p>
      <b>configuration</b>
    </p>
    <p>
      An array of windows descriptor. Each description is made up of following members :
    </p>
    <p>
      - display-id ;
    </p>
    <p>
      - view-uri ;
    </p>
    <p>
      - view-settings of the last backup ;
    </p>
    <p>
      - manager-uri.
    </p>
  </body>
</html></richcontent>
</node>
</node>
</node>
<node BACKGROUND_COLOR="#ffcc33" COLOR="#333333" CREATED="1298452469336" FOLDED="true" HGAP="27" ID="ID_525216193" MODIFIED="1304695799114" STYLE="bubble" TEXT="rename-layout" VSHIFT="5">
<font NAME="SansSerif" SIZE="12"/>
<node BACKGROUND_COLOR="#cccc00" COLOR="#333333" CREATED="1298454250840" FOLDED="true" HGAP="33" ID="ID_371544090" MODIFIED="1298555633188" STYLE="bubble" VSHIFT="-1">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      rename a layout
    </p>
  </body>
</html></richcontent>
<font NAME="SansSerif" SIZE="12"/>
<attribute_layout NAME_WIDTH="33" VALUE_WIDTH="95"/>
<attribute NAME="in" VALUE="old-name:string"/>
<attribute NAME="in" VALUE="new-name:string"/>
<attribute NAME="out" VALUE="success:bool"/>
<node CREATED="1298501005055" HGAP="38" ID="ID_1991339576" MODIFIED="1298501495576" VSHIFT="1">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      The layout to rename could be the active one.
    </p>
    <p>
      
    </p>
    <p>
      <b>old-name</b>
    </p>
    <p>
      Name of targeted layout
    </p>
    <p>
      
    </p>
    <p>
      <b>new-name</b>
    </p>
    <p>
      New name of the targeted layout
    </p>
  </body>
</html></richcontent>
</node>
</node>
</node>
<node BACKGROUND_COLOR="#ffcc33" COLOR="#333333" CREATED="1298452473992" FOLDED="true" HGAP="28" ID="ID_1590056459" MODIFIED="1304695800786" STYLE="bubble" TEXT="delete-layout" VSHIFT="4">
<font NAME="SansSerif" SIZE="12"/>
<node BACKGROUND_COLOR="#cccc00" COLOR="#333333" CREATED="1298454250840" FOLDED="true" HGAP="33" ID="ID_1057773525" MODIFIED="1298555633189" STYLE="bubble" VSHIFT="-1">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      delete a layout by its name
    </p>
  </body>
</html></richcontent>
<font NAME="SansSerif" SIZE="12"/>
<attribute_layout NAME_WIDTH="33" VALUE_WIDTH="71"/>
<attribute NAME="in" VALUE="name:string"/>
<attribute NAME="out" VALUE="success:bool"/>
<node BACKGROUND_COLOR="#e8e8e8" COLOR="#333333" CREATED="1298500833658" HGAP="37" ID="ID_1502339482" MODIFIED="1298536355048" STYLE="bubble" TEXT="The named layout to delete can&apos;t be the active one"/>
</node>
</node>
<node BACKGROUND_COLOR="#ffcc33" COLOR="#333333" CREATED="1298452479440" FOLDED="true" HGAP="30" ID="ID_728993048" MODIFIED="1304695802282" STYLE="bubble" TEXT="add-layout" VSHIFT="6">
<font NAME="SansSerif" SIZE="12"/>
<node BACKGROUND_COLOR="#cccc00" COLOR="#333333" CREATED="1298454250840" FOLDED="true" HGAP="33" ID="ID_1615513973" MODIFIED="1298555633189" STYLE="bubble" VSHIFT="-1">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      add a new layout
    </p>
  </body>
</html></richcontent>
<font NAME="SansSerif" SIZE="12"/>
<attribute_layout NAME_WIDTH="33" VALUE_WIDTH="71"/>
<attribute NAME="in" VALUE="name:string"/>
<attribute NAME="out" VALUE="success:bool"/>
<node BACKGROUND_COLOR="#e8e8e8" COLOR="#333333" CREATED="1298459379352" HGAP="86" ID="ID_1405871900" MODIFIED="1298501213690" STYLE="bubble" TEXT="Add a new layout. The current layout remains active." VSHIFT="-12"/>
</node>
</node>
<node BACKGROUND_COLOR="#ffcc33" COLOR="#333333" CREATED="1298452486840" FOLDED="true" HGAP="36" ID="ID_1039646634" MODIFIED="1304695803922" STYLE="bubble" TEXT="list-layout" VSHIFT="5">
<font NAME="SansSerif" SIZE="12"/>
<node BACKGROUND_COLOR="#cccc00" COLOR="#333333" CREATED="1298454250840" FOLDED="true" HGAP="33" ID="ID_147505247" MODIFIED="1298555633190" STYLE="bubble" VSHIFT="-1">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      list the layouts available
    </p>
    <p>
      in the current session
    </p>
  </body>
</html></richcontent>
<font NAME="SansSerif" SIZE="12"/>
<attribute_layout NAME_WIDTH="33" VALUE_WIDTH="71"/>
<attribute NAME="out" VALUE="names:array"/>
<node BACKGROUND_COLOR="#e8e8e8" COLOR="#333333" CREATED="1298459477521" ID="ID_565025640" MODIFIED="1298464979697" STYLE="bubble" TEXT="List of names for layouts available in the session"/>
</node>
</node>
<node BACKGROUND_COLOR="#ffcc33" COLOR="#333333" CREATED="1298452496448" FOLDED="true" HGAP="37" ID="ID_1927661745" MODIFIED="1304695805346" STYLE="bubble" TEXT="modify-layout" VSHIFT="6">
<font NAME="SansSerif" SIZE="12"/>
<node BACKGROUND_COLOR="#cccc00" COLOR="#333333" CREATED="1298454250840" HGAP="33" ID="ID_1804125102" MODIFIED="1298502265796" STYLE="bubble" VSHIFT="-1">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      change the window type of a window
    </p>
    <p>
      specified by the view-uri
    </p>
  </body>
</html></richcontent>
<font NAME="SansSerif" SIZE="12"/>
<attribute_layout NAME_WIDTH="33" VALUE_WIDTH="109"/>
<attribute NAME="in" VALUE="view-uri:string"/>
<attribute NAME="in" VALUE="window-type:string"/>
</node>
</node>
<node BACKGROUND_COLOR="#ffcc33" COLOR="#333333" CREATED="1298460997873" FOLDED="true" HGAP="35" ID="ID_643311812" MODIFIED="1303919666112" STYLE="bubble" TEXT="backup-layout" VSHIFT="3">
<node BACKGROUND_COLOR="#cccc00" COLOR="#333333" CREATED="1298454250840" FOLDED="true" HGAP="33" ID="ID_335275222" MODIFIED="1298555633190" STYLE="bubble" VSHIFT="-1">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      backup a layout
    </p>
  </body>
</html></richcontent>
<font NAME="SansSerif" SIZE="12"/>
<attribute_layout NAME_WIDTH="33" VALUE_WIDTH="84"/>
<attribute NAME="in" VALUE="name:string"/>
<attribute NAME="out" VALUE="confirmed:bool"/>
<node BACKGROUND_COLOR="#e8e8e8" COLOR="#333333" CREATED="1298460074385" ID="ID_455335947" MODIFIED="1298502372788" STYLE="bubble" TEXT="Backup a layout to the global session table"/>
<node BACKGROUND_COLOR="#ffffff" COLOR="#6666ff" CREATED="1298460181953" ID="ID_1506769014" LINK="#ID_175963075" MODIFIED="1298499343158" STYLE="fork" TEXT="backup-window (for each window)"/>
</node>
</node>
<node BACKGROUND_COLOR="#ffcc33" COLOR="#333333" CREATED="1298452513576" FOLDED="true" HGAP="36" ID="ID_1856756731" MODIFIED="1303919667040" STYLE="bubble" TEXT="present-layout" VSHIFT="7">
<font NAME="SansSerif" SIZE="12"/>
<node BACKGROUND_COLOR="#cccc00" COLOR="#333333" CREATED="1298454250840" HGAP="33" ID="ID_1763940501" MODIFIED="1303917119060" STYLE="bubble" VSHIFT="-1">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      present a whole layout
    </p>
  </body>
</html></richcontent>
<font NAME="SansSerif" SIZE="12"/>
<attribute_layout NAME_WIDTH="33" VALUE_WIDTH="84"/>
<attribute NAME="in" VALUE="name:string"/>
<attribute NAME="out" VALUE="confirmed:bool"/>
<node BACKGROUND_COLOR="#e8e8e8" COLOR="#333333" CREATED="1298460074385" ID="ID_847616420" MODIFIED="1298464979698" STYLE="bubble" TEXT="Present a layout, composed of several windows"/>
<node BACKGROUND_COLOR="#ffffff" COLOR="#6666ff" CREATED="1298460131464" ID="ID_1704230831" LINK="#ID_1986844325" MODIFIED="1298537957159" STYLE="fork" TEXT="create-window (for each window, if not exists)"/>
<node BACKGROUND_COLOR="#ffffff" COLOR="#6666ff" CREATED="1298460181953" ID="ID_199427689" LINK="#ID_1649779635" MODIFIED="1298537962910" STYLE="fork" TEXT="get-fl-frame (for each window, if not exists)"/>
</node>
</node>
<node BACKGROUND_COLOR="#ffcc33" COLOR="#333333" CREATED="1298452513576" FOLDED="true" HGAP="38" ID="ID_623524442" MODIFIED="1304695807986" STYLE="bubble" TEXT="show- overlay" VSHIFT="5">
<font NAME="SansSerif" SIZE="12"/>
<node BACKGROUND_COLOR="#cccc00" COLOR="#333333" CREATED="1298454250840" HGAP="33" ID="ID_149155696" MODIFIED="1303919400252" STYLE="bubble" VSHIFT="-1">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      Show an Overlay with image to the center
    </p>
  </body>
</html></richcontent>
<font NAME="SansSerif" SIZE="12"/>
<attribute_layout NAME_WIDTH="33" VALUE_WIDTH="84"/>
<attribute NAME="in" VALUE="image"/>
<attribute NAME="in" VALUE="uriView"/>
<node BACKGROUND_COLOR="#e8e8e8" COLOR="#333333" CREATED="1298460074385" ID="ID_565604960" MODIFIED="1303919414719" STYLE="bubble">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      <b>image</b>
    </p>
    <p>
      Image to show on the center of the overlay
    </p>
    <p>
      
    </p>
    <p>
      <b>uriView</b>
    </p>
    <p>
      it's on this view that overlay must be shown
    </p>
  </body>
</html></richcontent>
</node>
<node BACKGROUND_COLOR="#ffffff" COLOR="#6666ff" CREATED="1298460131464" ID="ID_154241964" LINK="#ID_86598239" MODIFIED="1303917196464" STYLE="fork" TEXT="overlay-shown( source )"/>
</node>
</node>
<node BACKGROUND_COLOR="#ffcc33" COLOR="#333333" CREATED="1298452513576" FOLDED="true" HGAP="38" ID="ID_174139316" MODIFIED="1304695812946" STYLE="bubble" TEXT="hide-overlay" VSHIFT="4">
<font NAME="SansSerif" SIZE="12"/>
<node BACKGROUND_COLOR="#cccc00" COLOR="#333333" CREATED="1298454250840" HGAP="33" ID="ID_783818757" MODIFIED="1303917286068" STYLE="bubble" VSHIFT="-1">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      Hide an overlay of the window matching with uriView
    </p>
  </body>
</html></richcontent>
<font NAME="SansSerif" SIZE="12"/>
<attribute_layout NAME_WIDTH="33" VALUE_WIDTH="84"/>
<attribute NAME="in" VALUE="uriView"/>
<node BACKGROUND_COLOR="#e8e8e8" COLOR="#333333" CREATED="1298460074385" ID="ID_843036345" MODIFIED="1303916907416" STYLE="bubble">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      <b>uriView</b>
    </p>
    <p>
      it's on this view that overlay must be shown
    </p>
  </body>
</html></richcontent>
</node>
</node>
</node>
</node>
<node BACKGROUND_COLOR="#333333" COLOR="#ccff00" CREATED="1298452387912" FOLDED="true" HGAP="81" ID="ID_1764211663" MODIFIED="1309186419759" POSITION="right" STYLE="bubble" TEXT="jam.asset-viewer" VSHIFT="13">
<edge COLOR="#808080" STYLE="bezier" WIDTH="2"/>
<font BOLD="true" NAME="SansSerif" SIZE="12"/>
<node BACKGROUND_COLOR="#990000" COLOR="#ccff00" CREATED="1298542706143" HGAP="40" ID="ID_1349402706" LINK="#ID_481131774" MODIFIED="1303917362283" STYLE="bubble" TEXT="is a window manager" VSHIFT="-15">
<font BOLD="true" NAME="SansSerif" SIZE="12"/>
</node>
<node BACKGROUND_COLOR="#ffcc33" COLOR="#333333" CREATED="1298452566976" FOLDED="true" HGAP="45" ID="ID_1912161175" MODIFIED="1303917363787" STYLE="bubble" TEXT="show-pak" VSHIFT="-2">
<font NAME="SansSerif" SIZE="12"/>
<node BACKGROUND_COLOR="#cccc00" COLOR="#333333" CREATED="1298454250840" FOLDED="true" HGAP="56" ID="ID_394384261" MODIFIED="1303907376132" STYLE="bubble" VSHIFT="11">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      introduce a way to explore
    </p>
    <p>
      the content of a .PAK
    </p>
  </body>
</html></richcontent>
<attribute_layout NAME_WIDTH="33" VALUE_WIDTH="114"/>
<attribute NAME="in" VALUE="pak-filename"/>
<node BACKGROUND_COLOR="#e8e8e8" COLOR="#333333" CREATED="1298454188160" HGAP="48" ID="ID_1460967011" MODIFIED="1303907372111" STYLE="bubble" VSHIFT="-5">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      <b>pak-filename</b>
    </p>
    <p>
      A string representing the pak filename to explore.
    </p>
  </body>
</html></richcontent>
</node>
</node>
</node>
<node BACKGROUND_COLOR="#333333" COLOR="#ccff00" CREATED="1298452387912" FOLDED="true" HGAP="49" ID="ID_1083795734" MODIFIED="1309186418767" STYLE="bubble" TEXT="jam.asset-viewer.view%d" VSHIFT="7">
<edge COLOR="#808080" STYLE="bezier" WIDTH="2"/>
<font BOLD="true" NAME="SansSerif" SIZE="12"/>
<node BACKGROUND_COLOR="#990000" COLOR="#ffffff" CREATED="1298542706143" ID="ID_1644023715" LINK="#ID_1502992799" MODIFIED="1298544286937" STYLE="bubble" TEXT="is a view">
<font BOLD="true" NAME="SansSerif" SIZE="12"/>
</node>
<node BACKGROUND_COLOR="#ffcc33" COLOR="#333333" CREATED="1298452566976" FOLDED="true" ID="ID_1420606232" MODIFIED="1303907384753" STYLE="bubble" TEXT="show-resource">
<font NAME="SansSerif" SIZE="12"/>
<node BACKGROUND_COLOR="#cccc00" COLOR="#333333" CREATED="1298454250840" FOLDED="true" HGAP="56" ID="ID_297921363" MODIFIED="1298555633193" STYLE="bubble" VSHIFT="11">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      select a resource&#160;to display
    </p>
  </body>
</html></richcontent>
<attribute_layout NAME_WIDTH="33" VALUE_WIDTH="113"/>
<attribute NAME="in" VALUE="resource-id:string"/>
<attribute NAME="in" VALUE="display-options:table"/>
<node BACKGROUND_COLOR="#e8e8e8" COLOR="#333333" CREATED="1298454188160" HGAP="48" ID="ID_622520897" MODIFIED="1298541762946" STYLE="bubble" VSHIFT="-5">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      Select a PAK resource to display.
    </p>
    <p>
      
    </p>
    <p>
      <b>resource-id</b>
    </p>
    <p>
      Valid Id of the resource in the PAK
    </p>
    <p>
      
    </p>
    <p>
      <b>display-options</b>
    </p>
    <p>
      Some options to customize the displaying.
    </p>
  </body>
</html></richcontent>
</node>
</node>
</node>
</node>
</node>
<node BACKGROUND_COLOR="#ffffff" COLOR="#999999" CREATED="1298542542070" FOLDED="true" HGAP="-15" ID="ID_528527551" MODIFIED="1298555782084" POSITION="left" STYLE="fork" TEXT="readme" VSHIFT="-65">
<font ITALIC="true" NAME="SansSerif" SIZE="10"/>
<node CREATED="1298537306080" HGAP="72" ID="ID_782469596" MODIFIED="1298544526736" VSHIFT="-54">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      <u>Example :</u>
    </p>
    <p>
      
    </p>
    <p>
      POST:
    </p>
    <p>
      <b>&#160;&#160;subject</b>='add-layout'
    </p>
    <p>
      &#160; <b>target</b>='jam.layout-man'
    </p>
    <p>
      
    </p>
    <p>
      REPLY:
    </p>
    <p>
      <b>&#160;&#160;subject</b>='<font color="#ff3333">reply-</font>add-layout'
    </p>
    <p>
      <b>&#160;&#160;source</b>='jam.layout-man'
    </p>
    <p>
      
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1298539119646" HGAP="71" ID="ID_579567199" MODIFIED="1298544514054" VSHIFT="25">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      <u>Caution:</u>
    </p>
    <p>
      
    </p>
    <p>
      Subject of replied message are automatically prefixed
    </p>
    <p>
      with the 'reply-' string by the facade:replyMessage() function.
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1298544331287" HGAP="66" ID="ID_1643636832" MODIFIED="1298544515990" VSHIFT="49">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      <u>Conventions:</u>
    </p>
    <p>
      
    </p>
    <p>
      Private messages (exchanged between a controller and others mvc components belonged) must prefix the subject with the 'private-'.
    </p>
  </body>
</html></richcontent>
</node>
</node>
<node BACKGROUND_COLOR="#990000" COLOR="#ccff00" CREATED="1298542257359" HGAP="52" ID="ID_990357873" MODIFIED="1308043148389" POSITION="left" STYLE="bubble" TEXT="mvc component class" VSHIFT="-81">
<font BOLD="true" NAME="SansSerif" SIZE="12"/>
<node BACKGROUND_COLOR="#ffcc33" COLOR="#333333" CREATED="1298452527096" HGAP="64" ID="ID_1747658218" MODIFIED="1298542373847" STYLE="bubble" TEXT="app-wakeup" VSHIFT="-1">
<font NAME="SansSerif" SIZE="12"/>
</node>
<node BACKGROUND_COLOR="#ffcc33" COLOR="#333333" CREATED="1298452527096" FOLDED="true" HGAP="64" ID="ID_430488743" MODIFIED="1308043150423" STYLE="bubble" TEXT="reply-*" VSHIFT="52">
<font NAME="SansSerif" SIZE="12"/>
<node BACKGROUND_COLOR="#e8e8e8" COLOR="#333333" CREATED="1298461281017" HGAP="74" ID="ID_1751422390" MODIFIED="1298537225729" STYLE="bubble" VSHIFT="-17">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      A confirmation reply.
    </p>
    <p>
      The subject of this kind of message must be prefixed by the 'reply-' string.
    </p>
    <p>
      The facade object is in charge to prepend the subject on a replyMessage() call.
    </p>
  </body>
</html></richcontent>
</node>
</node>
</node>
<node BACKGROUND_COLOR="#990000" COLOR="#ccff00" CREATED="1298452387912" HGAP="56" ID="ID_481131774" MODIFIED="1304695899213" POSITION="left" STYLE="bubble" TEXT="window manager class" VSHIFT="-7">
<edge COLOR="#808080" STYLE="bezier" WIDTH="2"/>
<font BOLD="true" NAME="SansSerif" SIZE="12"/>
<node BACKGROUND_COLOR="#ffcc33" COLOR="#333333" CREATED="1298452566976" FOLDED="true" ID="ID_1986844325" MODIFIED="1303919652496" STYLE="bubble" TEXT="create-window">
<font NAME="SansSerif" SIZE="12"/>
<node BACKGROUND_COLOR="#cccc00" COLOR="#333333" CREATED="1298454250840" FOLDED="true" HGAP="77" ID="ID_1848781003" MODIFIED="1303906638867" STYLE="bubble" VSHIFT="23">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      instanciate a window-type with or without
    </p>
    <p>
      a bring back of its serialized state
    </p>
  </body>
</html></richcontent>
<attribute_layout NAME_WIDTH="33" VALUE_WIDTH="114"/>
<attribute NAME="in" VALUE="window-type"/>
<attribute NAME="in" VALUE="request-id"/>
<attribute NAME="in*" VALUE="representation:table"/>
<attribute NAME="out" VALUE="view-uri"/>
<attribute NAME="out" VALUE="request-id"/>
<node BACKGROUND_COLOR="#e8e8e8" COLOR="#333333" CREATED="1298454188160" HGAP="48" ID="ID_802402040" MODIFIED="1298543808074" STYLE="bubble" VSHIFT="-5">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      <b>window-type</b>
    </p>
    <p>
      A string representing a unique type of window.
    </p>
    <p>
      
    </p>
    <p>
      <b>request-id</b>
    </p>
    <p>
      A numerical id identifying the request
    </p>
    <p>
      
    </p>
    <p>
      <b>representation</b>
    </p>
    <p>
      An optionnal table resulting from the serialization of the window state and aiming to provide all informations to bring its context back
    </p>
    <p>
      
    </p>
    <p>
      <b>view-uri</b>
    </p>
    <p>
      The uri of the newly created view (as a MVC component)
    </p>
  </body>
</html></richcontent>
</node>
</node>
</node>
<node BACKGROUND_COLOR="#ffcc33" COLOR="#333333" CREATED="1298452566976" FOLDED="true" ID="ID_1901404426" MODIFIED="1303919653072" STYLE="bubble" TEXT="delete-window">
<font NAME="SansSerif" SIZE="12"/>
<node BACKGROUND_COLOR="#cccc00" COLOR="#333333" CREATED="1298454250840" FOLDED="true" HGAP="77" ID="ID_589817871" MODIFIED="1298555782085" STYLE="bubble" VSHIFT="23">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      destroy a window
    </p>
  </body>
</html></richcontent>
<attribute_layout NAME_WIDTH="33" VALUE_WIDTH="114"/>
<attribute NAME="in" VALUE="view-uri:string"/>
<node BACKGROUND_COLOR="#e8e8e8" COLOR="#333333" CREATED="1298454188160" HGAP="48" ID="ID_1297915995" MODIFIED="1298544100320" STYLE="bubble" VSHIFT="-5">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      <b>view-uri</b>
    </p>
    <p>
      The uri of the view to delete (as a MVC component)
    </p>
  </body>
</html></richcontent>
</node>
</node>
</node>
<node BACKGROUND_COLOR="#ffcc33" COLOR="#333333" CREATED="1298452576408" FOLDED="true" ID="ID_175963075" MODIFIED="1303919653712" STYLE="bubble" TEXT="backup-window">
<font NAME="SansSerif" SIZE="12"/>
<node BACKGROUND_COLOR="#cccc00" COLOR="#333333" CREATED="1298454250840" FOLDED="true" HGAP="77" ID="ID_963079357" MODIFIED="1303906638099" STYLE="bubble" VSHIFT="23">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      serialize a window state
    </p>
  </body>
</html></richcontent>
<attribute_layout NAME_WIDTH="33" VALUE_WIDTH="114"/>
<attribute NAME="in" VALUE="view-uri:string"/>
<attribute NAME="out" VALUE="representation:table"/>
<node BACKGROUND_COLOR="#e8e8e8" COLOR="#333333" CREATED="1298454188160" HGAP="132" ID="ID_1088416040" MODIFIED="1298500512170" STYLE="bubble">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      <b>state</b>
    </p>
    <p>
      A LUA table resulting from the serialization of the window state
    </p>
    <p>
      
    </p>
    <p>
      <b>representation</b>
    </p>
    <p>
      A table resulting from the serialization of the window state and aiming to provide all informations to bring its context back
    </p>
  </body>
</html></richcontent>
</node>
</node>
</node>
<node BACKGROUND_COLOR="#ffcc33" CREATED="1303906282756" FOLDED="true" ID="ID_24608869" MODIFIED="1303919654336" TEXT="duplicate-window">
<node BACKGROUND_COLOR="#cccc00" COLOR="#333333" CREATED="1298454250840" FOLDED="true" HGAP="77" ID="ID_849101578" MODIFIED="1303916625441" STYLE="bubble" VSHIFT="23">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      duplicate a window from another
    </p>
  </body>
</html></richcontent>
<attribute_layout NAME_WIDTH="33" VALUE_WIDTH="114"/>
<attribute NAME="in" VALUE="window-type"/>
<attribute NAME="in" VALUE="request-id"/>
<attribute NAME="in" VALUE="view-uri-to-duplicate"/>
<attribute NAME="out" VALUE="view-uri"/>
<attribute NAME="out" VALUE="request-id"/>
<node BACKGROUND_COLOR="#e8e8e8" COLOR="#333333" CREATED="1298454188160" HGAP="132" ID="ID_926181156" MODIFIED="1303906630605" STYLE="bubble">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      <b>window-type</b>
    </p>
    <p>
      A string representing a unique type of window.
    </p>
    <p>
      
    </p>
    <p>
      <b>request-id</b>
    </p>
    <p>
      A numerical id identifying the request
    </p>
    <p>
      
    </p>
    <p>
      <b>view-uri-to-duplicate</b>
    </p>
    <p>
      Uri of the view we want to duplicate
    </p>
    <p>
      
    </p>
    <p>
      <b>view-uri</b>
    </p>
    <p>
      The uri of the newly created view (as a MVC component)
    </p>
  </body>
</html></richcontent>
</node>
</node>
</node>
<node BACKGROUND_COLOR="#ffcc33" COLOR="#333333" CREATED="1298454216752" FOLDED="true" HGAP="22" ID="ID_1305770875" MODIFIED="1308043091338" STYLE="bubble" TEXT="collect-window-managers" VSHIFT="-2">
<node BACKGROUND_COLOR="#cccc00" COLOR="#333333" CREATED="1298454250840" HGAP="49" ID="ID_252505455" MODIFIED="1303919712741" STYLE="bubble" TEXT="single" VSHIFT="12">
<attribute_layout NAME_WIDTH="33" VALUE_WIDTH="123"/>
<attribute NAME="out" VALUE="manager-uri:string"/>
<attribute NAME="out" VALUE="window-type-list:array"/>
<node BACKGROUND_COLOR="#e8e8e8" COLOR="#333333" CREATED="1298454188160" HGAP="157" ID="ID_1030150545" MODIFIED="1298538851361" STYLE="bubble" VSHIFT="-4">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      <b>manager-uri</b>
    </p>
    <p>
      The uri string (as a MVC component)
    </p>
    <p>
      
    </p>
    <p>
      <b>window-type-array</b>
    </p>
    <p>
      List of supported window types
    </p>
  </body>
</html></richcontent>
</node>
</node>
</node>
<node BACKGROUND_COLOR="#990000" COLOR="#ffffff" CREATED="1298542763007" ID="ID_683683759" LINK="#ID_990357873" MODIFIED="1298542790358" STYLE="bubble" TEXT="is a mvc component">
<font BOLD="true" NAME="SansSerif" SIZE="12"/>
</node>
</node>
<node BACKGROUND_COLOR="#990000" COLOR="#ccff00" CREATED="1298460154912" HGAP="69" ID="ID_1502992799" MODIFIED="1308043110302" POSITION="left" STYLE="bubble" TEXT="view class" VSHIFT="71">
<edge COLOR="#808080" STYLE="bezier" WIDTH="2"/>
<font BOLD="true" NAME="SansSerif" SIZE="12"/>
<node BACKGROUND_COLOR="#ffcc33" COLOR="#333333" CREATED="1298460159865" FOLDED="true" HGAP="82" ID="ID_1649779635" MODIFIED="1309791185668" STYLE="bubble" TEXT="get-fl-frame" VSHIFT="34">
<node BACKGROUND_COLOR="#cccc00" COLOR="#333333" CREATED="1298453193472" FOLDED="true" HGAP="51" ID="ID_306006058" MODIFIED="1303919808320" STYLE="bubble" TEXT="return the fl widget that will be attach" VSHIFT="12">
<attribute_layout NAME_WIDTH="33" VALUE_WIDTH="83"/>
<attribute NAME="out" VALUE="Fl_Widget*:ud"/>
<node BACKGROUND_COLOR="#e8e8e8" COLOR="#333333" CREATED="1298454188160" HGAP="157" ID="ID_539409792" MODIFIED="1303919749718" STYLE="bubble" VSHIFT="-4">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      <b>Fl_Widget*:ud</b>
    </p>
    <p>
      Pointer to the frame fltk of the view
    </p>
  </body>
</html></richcontent>
</node>
</node>
</node>
<node BACKGROUND_COLOR="#ffcc33" COLOR="#333333" CREATED="1298460159865" FOLDED="true" HGAP="87" ID="ID_877237268" MODIFIED="1308298158250" STYLE="bubble" TEXT="who-is-view" VSHIFT="9">
<node BACKGROUND_COLOR="#cccc00" COLOR="#333333" CREATED="1298453193472" FOLDED="true" HGAP="51" ID="ID_1676105216" MODIFIED="1308298151051" STYLE="bubble" TEXT="Return who is the view for x,y coordinate" VSHIFT="12">
<attribute_layout NAME_WIDTH="33" VALUE_WIDTH="79"/>
<attribute NAME="in" VALUE="x"/>
<attribute NAME="in" VALUE="y"/>
<attribute NAME="out" VALUE="Fl_Widget*:ud"/>
<node BACKGROUND_COLOR="#e8e8e8" COLOR="#333333" CREATED="1298454188160" HGAP="157" ID="ID_1642893729" MODIFIED="1303920845096" STYLE="bubble" VSHIFT="-4">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      <b>x</b>
    </p>
    <p>
      position mouse X
    </p>
    <p>
      
    </p>
    <p>
      <b>y</b>
    </p>
    <p>
      position mouse Y
    </p>
    <p>
      
    </p>
    <p>
      <b>Fl_Widget*:ud</b>
    </p>
    <p>
      widget that has been clocked
    </p>
  </body>
</html></richcontent>
</node>
</node>
</node>
<node BACKGROUND_COLOR="#ffcc33" COLOR="#333333" CREATED="1298452513576" FOLDED="true" HGAP="80" ID="ID_86598239" MODIFIED="1308043423042" STYLE="bubble" TEXT="overlay-shown" VSHIFT="6">
<font NAME="SansSerif" SIZE="12"/>
<node BACKGROUND_COLOR="#cccc00" COLOR="#333333" CREATED="1298454250840" HGAP="33" ID="ID_235709326" MODIFIED="1303917211172" STYLE="bubble" VSHIFT="-1">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      Notify that overlay has been showned (it's the answer to show-overlay)
    </p>
  </body>
</html></richcontent>
<font NAME="SansSerif" SIZE="12"/>
<attribute_layout NAME_WIDTH="33" VALUE_WIDTH="84"/>
</node>
</node>
<node BACKGROUND_COLOR="#ffcc33" COLOR="#333333" CREATED="1298452513576" FOLDED="true" HGAP="80" ID="ID_790465706" MODIFIED="1309509222140" STYLE="bubble" TEXT="on-drag" VSHIFT="6">
<font NAME="SansSerif" SIZE="12"/>
<node BACKGROUND_COLOR="#cccc00" COLOR="#333333" CREATED="1298453193472" HGAP="51" ID="ID_148685271" MODIFIED="1309509186567" STYLE="bubble" TEXT="Notify drag and drop" VSHIFT="12">
<attribute_layout NAME_WIDTH="33" VALUE_WIDTH="79"/>
<attribute NAME="in" VALUE="x"/>
<attribute NAME="in" VALUE="y"/>
<node BACKGROUND_COLOR="#e8e8e8" COLOR="#333333" CREATED="1298454188160" HGAP="157" ID="ID_540510405" MODIFIED="1309509190873" STYLE="bubble" VSHIFT="-4">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      <b>x </b>
    </p>
    <p>
      location of the mouse to determinate if this drag concerns the view
    </p>
    <p>
      
    </p>
    <p>
      <b>y</b>
    </p>
    <p>
      location of the mouse to determinate if this drag concerns the view
    </p>
  </body>
</html></richcontent>
</node>
</node>
</node>
<node BACKGROUND_COLOR="#ffcc33" COLOR="#333333" CREATED="1298452513576" FOLDED="true" HGAP="88" ID="ID_783753822" MODIFIED="1309791199030" STYLE="bubble" TEXT="on-drag-release" VSHIFT="-4">
<font NAME="SansSerif" SIZE="12"/>
<node BACKGROUND_COLOR="#cccc00" COLOR="#333333" CREATED="1298453193472" HGAP="51" ID="ID_1164413424" MODIFIED="1309509198528" STYLE="bubble" TEXT="Notify drag and drop release" VSHIFT="12">
<attribute_layout NAME_WIDTH="33" VALUE_WIDTH="79"/>
<attribute NAME="in" VALUE="x"/>
<attribute NAME="in" VALUE="y"/>
<attribute NAME="in" VALUE="string-data"/>
<attribute NAME="in " VALUE="idCategory"/>
<node BACKGROUND_COLOR="#e8e8e8" COLOR="#333333" CREATED="1298454188160" HGAP="157" ID="ID_83203203" MODIFIED="1308299035963" STYLE="bubble" VSHIFT="-4">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      <b>x </b>
    </p>
    <p>
      location of the mouse to determinate if this drag concerns the view
    </p>
    <p>
      
    </p>
    <p>
      <b>y</b>
    </p>
    <p>
      location of the mouse to determinate if this drag concerns the view
    </p>
    <p>
      
    </p>
    <p>
      
    </p>
    <p>
      <b>id-category</b>
    </p>
    <p>
      id of the category from where comes the drag and drop
    </p>
    <p>
      
    </p>
    <p>
      <b>string-data</b>
    </p>
    <p>
      essential information of the drag &amp; drop
    </p>
  </body>
</html></richcontent>
</node>
</node>
</node>
<node BACKGROUND_COLOR="#ffcc33" COLOR="#333333" CREATED="1298452527096" FOLDED="true" HGAP="87" ID="ID_751136618" MODIFIED="1309791244807" STYLE="bubble" TEXT="console-command" VSHIFT="-8">
<font NAME="SansSerif" SIZE="12"/>
<node BACKGROUND_COLOR="#cccc00" COLOR="#333333" CREATED="1298453193472" HGAP="63" ID="ID_683855319" MODIFIED="1309791227547" STYLE="bubble" TEXT="require a console command" VSHIFT="-20">
<attribute_layout NAME_WIDTH="33" VALUE_WIDTH="67"/>
<attribute NAME="in" VALUE="string"/>
<node CREATED="1298498958799" ID="ID_975175691" MODIFIED="1309791243038">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      <b>string</b>
    </p>
    <p>
      string of the command to execute
    </p>
  </body>
</html>
</richcontent>
</node>
</node>
</node>
<node BACKGROUND_COLOR="#990000" COLOR="#ffffff" CREATED="1298542763007" HGAP="76" ID="ID_1587180543" LINK="#ID_990357873" MODIFIED="1309791206744" STYLE="bubble" TEXT="is a mvc component" VSHIFT="-16">
<font BOLD="true" NAME="SansSerif" SIZE="12"/>
</node>
</node>
<node BACKGROUND_COLOR="#333333" COLOR="#ccff00" CREATED="1298452381992" HGAP="84" ID="ID_589032440" MODIFIED="1309186393968" POSITION="right" STYLE="bubble" TEXT="jam.db-man" VSHIFT="17">
<edge COLOR="#808080" STYLE="bezier" WIDTH="2"/>
<font BOLD="true" NAME="SansSerif" SIZE="12"/>
<node BACKGROUND_COLOR="#ffcc33" COLOR="#333333" CREATED="1298452527096" FOLDED="true" ID="ID_1386099964" MODIFIED="1309791133158" STYLE="bubble" TEXT="open-db">
<font NAME="SansSerif" SIZE="12"/>
<node BACKGROUND_COLOR="#cccc00" COLOR="#333333" CREATED="1298453193472" HGAP="63" ID="ID_171699801" MODIFIED="1304527336594" STYLE="bubble" TEXT="open a database" VSHIFT="-20">
<attribute_layout NAME_WIDTH="33" VALUE_WIDTH="67"/>
<attribute NAME="in" VALUE="path"/>
<node BACKGROUND_COLOR="#e8e8e8" COLOR="#333333" CREATED="1298498958799" ID="ID_686783733" MODIFIED="1304527358194" STYLE="bubble">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      <b>path</b>
    </p>
    <p>
      open the database of this path
    </p>
  </body>
</html></richcontent>
</node>
</node>
</node>
<node BACKGROUND_COLOR="#ffcc33" COLOR="#333333" CREATED="1298452542128" FOLDED="true" ID="ID_874489170" MODIFIED="1309791134023" STYLE="bubble" TEXT="close-db">
<font NAME="SansSerif" SIZE="12"/>
<node BACKGROUND_COLOR="#cccc00" COLOR="#333333" CREATED="1298453193472" HGAP="51" ID="ID_1256294170" MODIFIED="1304527372369" STYLE="bubble" TEXT="close the database" VSHIFT="12">
<attribute_layout NAME_WIDTH="33" VALUE_WIDTH="67"/>
</node>
</node>
<node BACKGROUND_COLOR="#ffcc33" COLOR="#333333" CREATED="1298452542128" FOLDED="true" ID="ID_280046876" MODIFIED="1304584046422" STYLE="bubble" TEXT="execute-sql">
<font NAME="SansSerif" SIZE="12"/>
<node BACKGROUND_COLOR="#cccc00" COLOR="#333333" CREATED="1298453193472" HGAP="51" ID="ID_284698122" MODIFIED="1304527752967" STYLE="bubble" TEXT="Excutes are send to the DB and transation is finished" VSHIFT="12">
<attribute_layout NAME_WIDTH="33" VALUE_WIDTH="67"/>
</node>
</node>
<node BACKGROUND_COLOR="#ffcc33" COLOR="#333333" CREATED="1298452542128" FOLDED="true" ID="ID_760682595" MODIFIED="1309186426463" STYLE="bubble" TEXT="flush">
<font NAME="SansSerif" SIZE="12"/>
<node BACKGROUND_COLOR="#cccc00" COLOR="#333333" CREATED="1298453193472" HGAP="51" ID="ID_1681279876" MODIFIED="1305900455619" STYLE="bubble" TEXT="Synchronize Buffers and Hook" VSHIFT="12">
<attribute_layout NAME_WIDTH="33" VALUE_WIDTH="67"/>
</node>
</node>
</node>
<node BACKGROUND_COLOR="#333333" COLOR="#ccff00" CREATED="1298452381992" HGAP="84" ID="ID_939045729" MODIFIED="1309186390592" POSITION="right" STYLE="bubble" TEXT="jam.asset-man" VSHIFT="17">
<edge COLOR="#808080" STYLE="bezier" WIDTH="2"/>
<font BOLD="true" NAME="SansSerif" SIZE="12"/>
<node BACKGROUND_COLOR="#ffcc33" COLOR="#333333" CREATED="1298452542128" FOLDED="true" ID="ID_1341613933" MODIFIED="1308043154074" STYLE="bubble" TEXT="generate-pak-from-sua">
<font NAME="SansSerif" SIZE="12"/>
<node BACKGROUND_COLOR="#cccc00" COLOR="#333333" CREATED="1298453193472" HGAP="51" ID="ID_1312778886" MODIFIED="1304689785901" STYLE="bubble" VSHIFT="12">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      Generate a Pak from a SUA File
    </p>
  </body>
</html></richcontent>
<attribute_layout NAME_WIDTH="33" VALUE_WIDTH="67"/>
</node>
</node>
</node>
</node>
</map>
