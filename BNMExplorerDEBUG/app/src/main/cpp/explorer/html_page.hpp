#pragma once
#include <string>

inline std::string get_explorer_html(int asm_count) {
    std::string html = R"HTML(<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width,initial-scale=1.0,maximum-scale=1.0,user-scalable=no">
<title>BNM Explorer</title>
<link rel="preconnect" href="https://fonts.googleapis.com">
<link rel="preconnect" href="https://fonts.gstatic.com" crossorigin>
<link href="https://fonts.googleapis.com/css2?family=Montserrat:ital,wght@0,300;0,400;0,500;0,600;0,700;0,800;1,400&family=Material+Symbols+Rounded:opsz,wght,FILL,GRAD@20..48,700,1,0&display=swap" rel="stylesheet">
<style>
:root{
  --bg:#0f0f0f;--bg2:#161616;--bg3:#1e1e1e;--bg4:#2a2a2a;--card:#191919;
  --sel:#3a3a3a;--sel-hover:#484848;--sel2:#5a5a5a;--fg:#eeeeee;--fg-dim:#8c8c8c;
  --kw:#d4d4d4;--type:#34d399;--mth:#fbbf24;--str:#f87171;--num:#a78bfa;
  --brd:#2e2e2e;--brd-light:rgba(255,255,255,0.08);
  --red:#262626;--red2:#3d3d3d;--grn:#262626;--grn2:#3d3d3d;
  --org:#262626;--org2:#3d3d3d;--blu:#262626;--blu2:#3d3d3d;--purp:#262626;
  --rad-sm:6px;--rad-md:10px;--rad-lg:14px;--rad-xl:18px;
}
*{box-sizing:border-box;margin:0;padding:0;font-family:'Montserrat',system-ui,-apple-system,sans-serif;-webkit-tap-highlight-color:transparent;}
body{background:var(--bg);color:var(--fg);display:flex;flex-direction:column;height:100vh;overflow:hidden;font-size:12px;}
.g-icon{font-family:'Material Symbols Rounded',sans-serif;font-weight:700;font-style:normal;font-size:16px;line-height:1;display:inline-flex;align-items:center;justify-content:center;text-transform:none;letter-spacing:normal;word-wrap:normal;white-space:nowrap;direction:ltr;-webkit-font-smoothing:antialiased;font-variation-settings:'FILL' 1,'wght' 700,'GRAD' 0,'opsz' 24;color:#fff;flex-shrink:0;}
.toolbar{background:var(--bg2);padding:6px 12px;border-bottom:1px solid var(--brd);display:flex;gap:10px;align-items:center;z-index:20;}
.brand{display:flex;align-items:center;gap:8px;font-weight:700;font-size:13px;letter-spacing:0.3px;color:#fff;}
.badge{background:var(--bg3);border:1px solid var(--brd);padding:2px 6px;border-radius:var(--rad-sm);font-size:10px;font-weight:600;color:var(--fg-dim);}
.workspace{display:flex;flex:1;overflow:hidden;padding:6px;gap:6px;position:relative;}
.sidebar-overlay{display:none;position:fixed;top:0;left:0;right:0;bottom:0;background:rgba(0,0,0,0.65);backdrop-filter:blur(3px);z-index:40;}
.sidebar{width:290px;background:var(--bg2);border:1px solid var(--brd);border-radius:var(--rad-lg);display:flex;flex-direction:column;overflow:hidden;flex-shrink:0;transition:transform .2s ease;}
.side-hdr{background:var(--bg3);padding:8px 12px;border-bottom:1px solid var(--brd);color:#fff;font-weight:600;display:flex;justify-content:space-between;align-items:center;}
.tree-wrap{flex:1;overflow-y:auto;padding:6px 4px;}
::-webkit-scrollbar{width:7px;height:7px;}
::-webkit-scrollbar-thumb{background:var(--bg4);border-radius:4px;}
::-webkit-scrollbar-track{background:transparent;}
.tree-node{list-style:none;padding-left:10px;}
.tree-nc{display:flex;align-items:center;padding:4px 6px;cursor:pointer;white-space:nowrap;user-select:none;border-radius:var(--rad-sm);margin-bottom:1px;transition:background .12s;}
.tree-nc:hover{background:var(--bg3);}
.tree-nc.active{background:var(--sel);color:#fff;font-weight:600;}
.arr{display:inline-flex;align-items:center;justify-content:center;width:14px;height:14px;margin-right:2px;font-size:10px;color:var(--fg-dim);transition:transform .12s;}
.arr.open{transform:rotate(90deg);}
.ico{width:16px;height:16px;margin-right:6px;display:inline-flex;align-items:center;justify-content:center;flex-shrink:0;}
.ico svg{width:100%;height:100%;fill:#fff;}
.kids{display:none;}
.kids.open{display:block;}
.main-area{flex:1;display:flex;flex-direction:column;overflow:hidden;border:1px solid var(--brd);border-radius:var(--rad-lg);background:var(--bg2);}
.tabs{display:flex;background:var(--bg2);border-bottom:1px solid var(--brd);overflow-x:auto;padding:4px 6px 0 6px;gap:3px;}
.tab{padding:6px 12px;background:transparent;border:1px solid transparent;border-bottom:none;cursor:pointer;border-radius:var(--rad-md) var(--rad-md) 0 0;white-space:nowrap;flex-shrink:0;color:var(--fg-dim);font-weight:600;font-size:11px;display:flex;align-items:center;gap:6px;transition:all .15s;}
.tab:hover{background:var(--bg3);color:#fff;}
.tab.active{background:var(--bg3);color:#fff;border-color:var(--brd);border-top:2px solid var(--sel2);}
.panel{flex:1;overflow:auto;background:var(--bg2);display:none;}
.panel.active{display:flex;flex-direction:column;}
.scene-mgr{display:flex;flex:1;overflow:hidden;}
.hier{width:330px;border-right:1px solid var(--brd);display:flex;flex-direction:column;background:var(--bg2);flex-shrink:0;}
.hier-hdr{background:var(--bg3);padding:8px 10px;border-bottom:1px solid var(--brd);display:flex;flex-direction:column;gap:6px;}
.hier-title-row{display:flex;justify-content:space-between;align-items:center;}
.insp{flex:1;overflow-y:auto;background:var(--bg);}
.insp-hdr{padding:8px 12px;border-bottom:1px solid var(--brd);display:flex;align-items:center;gap:8px;background:var(--bg3);position:sticky;top:0;z-index:10;}
.insp-hdr .inp{flex:1;font-size:12px;font-weight:700;background:transparent;border:1px solid transparent;border-radius:var(--rad-sm);}
.insp-hdr .inp:focus{border:1px solid var(--sel2);background:var(--bg2);}
.comp{border-bottom:1px solid var(--brd);background:var(--bg2);margin-bottom:6px;border-radius:var(--rad-md);overflow:hidden;border:1px solid var(--brd);}
.comp-hdr{background:var(--bg3);padding:6px 10px;display:flex;align-items:center;cursor:pointer;user-select:none;gap:6px;font-weight:600;}
.comp-hdr:hover{background:var(--bg4);}
.comp-body{display:none;padding:2px 0;}
.comp-body.open{display:block;}
.row{display:flex;align-items:center;border-bottom:1px solid var(--brd-light);min-height:30px;}
.row:last-child{border-bottom:none;}
.lbl{width:36%;overflow:hidden;text-overflow:ellipsis;white-space:nowrap;padding:4px 8px;color:var(--type);border-right:1px solid var(--brd-light);flex-shrink:0;font-size:11px;font-weight:500;}
.val{flex:1;display:flex;gap:4px;padding:4px 8px;align-items:center;flex-wrap:wrap;}
.vl{color:var(--fg-dim);margin-right:2px;font-size:10px;font-weight:600;}
.code-view{font-family:'JetBrains Mono',Consolas,'Courier New',monospace;font-size:12px;line-height:1.6;padding:12px 16px;white-space:pre;}
.c-kw{color:var(--kw);font-family:inherit;}
.c-ty{color:var(--type);font-family:inherit;}
.c-mt{color:var(--mth);font-family:inherit;}
.c-st{color:var(--str);font-family:inherit;}
.c-cm{color:#7a7a7a;font-family:inherit;}
.c-nm{color:var(--num);font-family:inherit;}
.table{width:100%;border-collapse:separate;border-spacing:0;border-radius:var(--rad-md);overflow:hidden;border:1px solid var(--brd);}
.table th,.table td{padding:6px 10px;border-bottom:1px solid var(--brd);border-right:1px solid var(--brd);text-align:left;}
.table th{background:var(--bg3);font-weight:600;color:#fff;}
.table tr:last-child td{border-bottom:none;}
.table tr td:last-child,.table tr th:last-child{border-right:none;}
.btn{background:var(--bg3);color:#fff;border:1px solid var(--brd);padding:5px 10px;border-radius:var(--rad-md);cursor:pointer;display:inline-flex;align-items:center;justify-content:center;gap:5px;outline:none;flex-shrink:0;font-size:11px;font-weight:600;transition:all .12s;}
.btn:hover{background:var(--bg4);border-color:var(--fg-dim);}
.btn-red{background:var(--red);border-color:var(--red2);color:#f87171;}
.btn-red:hover{background:var(--red2);}
.btn-grn{background:var(--grn);border-color:var(--grn2);color:#34d399;}
.btn-grn:hover{background:var(--grn2);}
.btn-org{background:var(--org);border-color:var(--org2);color:#fbbf24;}
.btn-org:hover{background:var(--org2);}
.btn-blu{background:var(--blu);border-color:var(--blu2);color:#eeeeee;}
.btn-blu:hover{background:var(--blu2);}
.btn-live{background:var(--bg3);border-color:var(--grn2);color:#34d399;}
.btn-live.active{background:var(--grn);color:#34d399;}
.inp{background:var(--bg);border:1px solid var(--brd);color:#fff;padding:5px 8px;border-radius:var(--rad-md);width:100%;outline:none;font-size:11px;transition:border-color .15s;}
.inp:focus{border-color:var(--sel2);background:var(--bg2);}
.inp:disabled{opacity:.5;cursor:not-allowed;}
.srch{background:var(--bg);border:1px solid var(--brd);color:#fff;padding:5px 10px;border-radius:var(--rad-md);width:220px;outline:none;font-size:11px;}
.srch:focus{border-color:var(--sel2);}
.ctrl-res{font-family:Consolas,monospace;background:var(--bg);padding:4px 8px;border:1px solid var(--brd);border-radius:var(--rad-sm);flex:1;margin-left:6px;min-height:24px;display:flex;align-items:center;font-size:11px;}
.loop-row{display:flex;align-items:center;gap:6px;margin-bottom:4px;background:var(--card);padding:5px 10px;border:1px solid var(--brd);border-radius:var(--rad-md);}
.loop-id{font-family:Consolas,monospace;font-size:10px;flex:1;overflow:hidden;text-overflow:ellipsis;white-space:nowrap;color:var(--type);}
.script-hdr{background:#171717;}
.builtin-hdr{background:#171717;}
.catcher-entry{border-bottom:1px solid var(--brd);margin-bottom:6px;border-radius:var(--rad-md);overflow:hidden;border:1px solid var(--brd);}
.catcher-hdr{background:var(--bg3);padding:6px 10px;display:flex;align-items:center;gap:6px;cursor:pointer;user-select:none;font-weight:600;}
.catcher-hdr:hover{background:var(--bg4);}
.catcher-body{display:none;max-height:300px;overflow-y:auto;background:var(--bg);}
.catcher-body.open{display:block;}
.call-row{padding:5px 10px;border-bottom:1px solid var(--brd-light);font-family:Consolas,monospace;font-size:11px;display:flex;flex-direction:column;gap:2px;}
.call-row:hover{background:var(--bg3);}
.modal-overlay{position:fixed;top:0;left:0;right:0;bottom:0;background:rgba(0,0,0,.75);backdrop-filter:blur(4px);display:none;z-index:100;align-items:center;justify-content:center;padding:12px;}
.modal-overlay.open{display:flex;}
.modal-box{background:var(--bg2);border:1px solid var(--brd);border-radius:var(--rad-xl);width:460px;max-width:96vw;max-height:82vh;display:flex;flex-direction:column;overflow:hidden;box-shadow:0 12px 30px rgba(0,0,0,.6);}
.modal-hdr{padding:10px 14px;background:var(--bg3);border-bottom:1px solid var(--brd);display:flex;justify-content:space-between;align-items:center;font-weight:700;}
.modal-body{flex:1;overflow-y:auto;padding:8px;}
.add-comp-cat-hdr{padding:6px 10px;background:var(--bg3);border-bottom:1px solid var(--brd);cursor:pointer;font-weight:700;border-radius:var(--rad-sm);margin-bottom:2px;}
.add-comp-cat-hdr:hover{background:var(--bg4);}
.add-comp-items{display:none;margin-bottom:4px;}
.add-comp-items.open{display:block;}
.add-comp-item{padding:5px 14px;cursor:pointer;border-bottom:1px solid var(--brd-light);border-radius:var(--rad-sm);}
.add-comp-item:hover{background:var(--sel);color:#fff;}
.coll-row{padding:7px 10px;border-bottom:1px solid var(--brd-light);display:flex;align-items:center;gap:8px;border-radius:var(--rad-md);}
.coll-row.clk{cursor:pointer;}
.coll-row.clk:hover{background:var(--sel);}
.coll-idx{min-width:34px;color:var(--fg-dim);font-weight:700;font-family:Consolas,monospace;font-size:10px;word-break:break-all;}
.coll-val{flex:1;overflow:hidden;text-overflow:ellipsis;white-space:nowrap;color:var(--fg);}
.coll-ty{color:var(--type);font-size:10px;font-weight:600;white-space:nowrap;}
.inst-item{padding:8px 10px;cursor:pointer;border-bottom:1px solid var(--brd-light);border-radius:var(--rad-md);display:flex;align-items:center;justify-content:space-between;gap:8px;transition:background .12s;}
.inst-item:hover{background:var(--sel);color:#fff;}
.pulse-dot{width:8px;height:8px;border-radius:50%;background:#d9d9d9;box-shadow:0 0 6px rgba(217,217,217,.5);display:inline-block;margin-right:4px;}
.pulse-dot.off{background:#7a7a7a;box-shadow:none;}
.mono-tag{font-size:9px;background:var(--bg3);border:1px solid var(--brd);color:var(--fg-dim);padding:1px 5px;border-radius:var(--rad-sm);margin-left:4px;}
.mobile-only{display:none;}

@media (max-width:860px){
  .mobile-only{display:inline-flex;}
  .sidebar{position:fixed;top:44px;left:0;bottom:0;width:82vw;max-width:320px;z-index:50;transform:translateX(-100%);border-radius:0 var(--rad-xl) var(--rad-xl) 0;box-shadow:5px 0 25px rgba(0,0,0,.7);}
  .sidebar.open{transform:translateX(0);}
  .sidebar-overlay.open{display:block;}
  .scene-mgr{flex-direction:column;}
  .hier{width:100%;border-right:none;border-bottom:1px solid var(--brd);max-height:45vh;}
  .insp{flex:1;}
  .srch{width:140px;}
  .btn{padding:6px 10px;}
  .row{flex-direction:column;align-items:stretch;}
  .lbl{width:100%;border-right:none;border-bottom:1px solid var(--brd-light);padding:4px 8px;}
  .val{width:100%;padding:4px 8px;}
}
</style>
</head>
<body>
<div class="sidebar-overlay" id="sidebar-overlay" onclick="toggle_sidebar(false)"></div>
<div class="toolbar">
  <button class="btn mobile-only" onclick="toggle_sidebar(true)"><span class="g-icon">menu</span></button>
  <div class="brand">
    <span class="g-icon" style="font-size:18px;">deployed_code</span>
    <span>BNM Explorer</span>
    <span class="badge">)HTML";
    html += std::to_string(asm_count);
    html += R"HTML( asms</span>
  </div>
  <button class="btn btn-grn" onclick="download_dump()" style="margin-left:auto;"><span class="g-icon">download</span> Dump</button>
  <input type="text" class="srch" id="gSearch" placeholder="Search classes..." onkeyup="do_search(event)" style="margin-left:6px;">
</div>
<div class="workspace">
  <div class="sidebar" id="app-sidebar">
    <div class="side-hdr">
      <span>Object Explorer</span>
      <button class="btn mobile-only" style="padding:2px 6px;" onclick="toggle_sidebar(false)"><span class="g-icon">close</span></button>
    </div>
    <div class="tree-wrap" id="tree-root"></div>
  </div>
  <div class="main-area">
    <div class="tabs">
      <div class="tab active" onclick="switch_tab('scene')" id="tab-scene"><span class="g-icon">account_tree</span> Scene</div>
      <div class="tab" onclick="switch_tab('code')" id="tab-code"><span class="g-icon">code</span> Inspector</div>
      <div class="tab" onclick="switch_tab('inst')" id="tab-inst"><span class="g-icon">dataset</span> Instances</div>
      <div class="tab" onclick="switch_tab('ctrl')" id="tab-ctrl"><span class="g-icon">gamepad</span> Controller</div>
      <div class="tab" onclick="switch_tab('loops')" id="tab-loops"><span class="g-icon">sync</span> Loops</div>
      <div class="tab" onclick="switch_tab('catcher')" id="tab-catcher"><span class="g-icon">track_changes</span> Catcher</div>
      <div class="tab" onclick="switch_tab('lua')" id="tab-lua"><span class="g-icon">terminal</span> Console</div>
      <div class="tab" onclick="switch_tab('docs')" id="tab-docs"><span class="g-icon">description</span> Docs</div>
    </div>

    <div class="panel active" id="pnl-scene">
      <div class="scene-mgr">
        <div class="hier">
          <div class="hier-hdr">
            <div class="hier-title-row">
              <span style="font-weight:700;">Hierarchy</span>
              <div style="display:flex;gap:4px;">
                <button class="btn btn-live" id="btn-live-sync" onclick="toggle_live_sync()" title="Toggle live auto sync"><span class="pulse-dot" id="live-dot"></span> Live</button>
                <button class="btn" onclick="load_scene(true)" title="Reload Scene"><span class="g-icon">refresh</span></button>
              </div>
            </div>
            <input type="text" class="inp" id="sceneSearch" placeholder="Filter hierarchy..." oninput="filter_scene()">
          </div>
          <div class="tree-wrap" id="scene-tree"></div>
        </div>
        <div class="insp" id="scene-insp">
          <div style="padding:30px;text-align:center;color:var(--fg-dim);">Select a GameObject from the hierarchy</div>
        </div>
      </div>
    </div>

    <div class="panel code-view" id="pnl-code"></div>

    <div class="panel" id="pnl-inst" style="padding:0">
      <div class="scene-mgr">
        <div class="hier" style="width:100%;border:none;">
          <div class="hier-hdr"><span style="font-weight:700;">Class Instances</span></div>
          <div class="tree-wrap" id="inst-list" style="padding:10px;">Select a class in the explorer to view live instances</div>
        </div>
      </div>
    </div>

    <div class="panel" id="pnl-ctrl" style="padding:0">
      <div class="scene-mgr">
        <div class="hier" style="width:310px;">
          <div class="hier-hdr"><span style="font-weight:700;">Target Object</span></div>
          <div style="padding:10px;display:flex;flex-direction:column;gap:8px;">
            <div>
              <label style="color:var(--fg-dim);display:block;margin-bottom:3px;font-weight:600;">Address</label>
              <div style="display:flex;gap:4px;">
                <input class="inp" id="ctrl-addr" value="0x0">
                <button class="btn btn-blu" onclick="open_ctrl_instance_picker()" title="Pick live instance"><span class="g-icon">touch_app</span></button>
              </div>
            </div>
            <div style="display:flex;align-items:center;gap:6px;">
              <input type="checkbox" id="ctrl-static" onchange="document.getElementById('ctrl-addr').disabled=this.checked;if(this.checked)document.getElementById('ctrl-addr').value='0x0';">
              <label for="ctrl-static" style="color:var(--fg-dim);font-weight:600;">Static (no instance)</label>
            </div>
            <div><label style="color:var(--fg-dim);display:block;margin-bottom:3px;font-weight:600;">Assembly</label><input class="inp" id="ctrl-asm"></div>
            <div><label style="color:var(--fg-dim);display:block;margin-bottom:3px;font-weight:600;">Namespace</label><input class="inp" id="ctrl-ns"></div>
            <div><label style="color:var(--fg-dim);display:block;margin-bottom:3px;font-weight:600;">Class</label><input class="inp" id="ctrl-cls"></div>
            <button class="btn btn-grn" style="justify-content:center;" onclick="load_controller()"><span class="g-icon">search</span> Inspect Target</button>
          </div>
        </div>
        <div class="insp" id="ctrl-insp"><div style="padding:30px;text-align:center;color:var(--fg-dim);">Configure target or select from explorer</div></div>
      </div>
    </div>

    <div class="panel" id="pnl-loops" style="padding:10px;flex-direction:column;gap:6px;">
      <div style="display:flex;justify-content:space-between;align-items:center;background:var(--bg3);padding:8px 12px;border-radius:var(--rad-md);border:1px solid var(--brd);">
        <span style="font-weight:700;">Active Memory Loops</span>
        <button class="btn btn-red" onclick="remove_all_loops()"><span class="g-icon">delete</span> Stop All</button>
      </div>
      <div id="loops-list" style="margin-top:6px;"></div>
    </div>

    <div class="panel" id="pnl-catcher" style="padding:0;flex-direction:column;">
      <div style="background:var(--bg3);padding:8px 12px;border-bottom:1px solid var(--brd);display:flex;gap:8px;align-items:center;">
        <span style="font-weight:700;">Method Catcher</span>
        <span style="opacity:.6;font-size:11px;flex:1;">Intercept and log runtime calls</span>
        <button class="btn btn-red" onclick="unhook_all()"><span class="g-icon">delete</span> Unhook All</button>
      </div>
      <div id="catcher-list" style="flex:1;overflow-y:auto;padding:8px;"></div>
    </div>

    <div class="panel" id="pnl-lua" style="padding:0;flex-direction:column;">
      <div style="background:var(--bg3);padding:8px 12px;border-bottom:1px solid var(--brd);display:flex;gap:8px;align-items:center;">
        <span style="font-weight:700;">LuaBNM Console</span>
        <span style="opacity:.6;font-size:11px;flex:1;">Lua 5.1 + BNM bindings</span>
        <button class="btn btn-grn" onclick="run_lua()"><span class="g-icon">play_arrow</span> Run</button>
        <button class="btn btn-red" onclick="stop_lua()"><span class="g-icon">stop</span> Stop</button>
        <button class="btn btn-red" onclick="reset_lua()"><span class="g-icon">restart_alt</span> Reset</button>
        <button class="btn btn-red" onclick="clear_console()"><span class="g-icon">delete</span> Clear</button>
      </div>
      <div style="display:flex;flex:1;overflow:hidden;">
        <div style="flex:1;display:flex;flex-direction:column;border-right:1px solid var(--brd);">
          <div style="padding:4px 8px;background:var(--bg3);border-bottom:1px solid var(--brd);font-size:10px;color:var(--fg-dim);">Editor</div>
          <textarea id="lua-editor" spellcheck="false" style="flex:1;background:var(--bg);color:var(--fg);border:none;padding:10px;font-family:'JetBrains Mono',Consolas,'Courier New',monospace;font-size:12px;resize:none;outline:none;tab-size:2;" placeholder="-- Write LuaBNM code here..."></textarea>
        </div>
        <div style="flex:1;display:flex;flex-direction:column;">
          <div style="padding:4px 8px;background:var(--bg3);border-bottom:1px solid var(--brd);font-size:10px;color:var(--fg-dim);">Output</div>
          <pre id="lua-output" style="flex:1;background:#0a0a0a;color:#34d399;padding:10px;margin:0;overflow:auto;font-family:'JetBrains Mono',Consolas,'Courier New',monospace;font-size:11px;white-space:pre-wrap;"></pre>
        </div>
      </div>
    </div>

    <div class="panel" id="pnl-docs" style="padding:20px;flex-direction:column;overflow-y:auto;">
      <div style="max-width:760px;margin:0 auto;width:100%;">
        <h2 style="color:var(--kw);margin-bottom:8px;">LuaBNM Documentation</h2>
        <div style="color:var(--fg-dim);margin-bottom:16px;">Embedded Lua scripting runtime for IL2CPP/Unity games via BNM.</div>

                <h3 style="color:var(--type);margin:16px 0 8px;">Classes and Methods</h3>
        <pre style="background:var(--bg);padding:10px;border-radius:var(--rad-md);border:1px solid var(--brd);margin-bottom:12px;color:var(--fg);white-space:pre-wrap;">local cls = find_class("UnityEngine.GameObject")
print(cls:is_valid())
local method = cls:get_method("Find", 1)
print(method:is_valid(), method("Main Camera"))
local asm = assembly("Assembly-CSharp.dll")
local player = asm:namespace("MyGame"):class("Player")
print(player.some_static_field)</pre>

        <h3 style="color:var(--type);margin:16px 0 8px;">GameObject and Primitives</h3>
        <pre style="background:var(--bg);padding:10px;border-radius:var(--rad-md);border:1px solid var(--brd);margin-bottom:12px;color:var(--fg);white-space:pre-wrap;">local cam = game_object.Find("Main Camera")
local cube = game_object.CreatePrimitive(primitive_type.cube)
cube.transform.position = vec3(0, 2, 0)
cube.transform.localScale = vec3(0.3, 0.05, 0.4)
cube:destroy(2.0)
print(is_valid(cube))</pre>

        <h3 style="color:var(--type);margin:16px 0 8px;">Components and Physics</h3>
        <pre style="background:var(--bg);padding:10px;border-radius:var(--rad-md);border:1px solid var(--brd);margin-bottom:12px;color:var(--fg);white-space:pre-wrap;">local player = game_object.Find("Player")
local body = player:get_component("UnityEngine.Rigidbody")
body.velocity = vec3(0, 5, 0)
body:AddForce(vec3(0, 1, 0), force_mode.velocity_change)</pre>

        <h3 style="color:var(--type);margin:16px 0 8px;">Vectors and Colors</h3>
        <pre style="background:var(--bg);padding:10px;border-radius:var(--rad-md);border:1px solid var(--brd);margin-bottom:12px;color:var(--fg);white-space:pre-wrap;">local a = vec3(1, 2, 3)
local b = a * 2 + vec3(0, 1, 0)
print(b, b:length(), b:normalized(), a:dot(b))
local c = color(1, 0, 0, 1)
local q = quat(0, 0, 0, 1)</pre>

        <h3 style="color:var(--type);margin:16px 0 8px;">XR Input</h3>
        <pre style="background:var(--bg);padding:10px;border-radius:var(--rad-md);border:1px solid var(--brd);margin-bottom:12px;color:var(--fg);white-space:pre-wrap;">local grip = xr.get_bool_feature("grip_button", "right")
local trigger = xr.get_float_feature("trigger", "left")
local stick = xr.get_vector2_feature("primary_2d_axis", "right")
xr.send_haptic_impulse("right", 0.5, 0.1)</pre>

        <h3 style="color:var(--type);margin:16px 0 8px;">Scheduling</h3>
        <pre style="background:var(--bg);padding:10px;border-radius:var(--rad-md);border:1px solid var(--brd);margin-bottom:12px;color:var(--fg);white-space:pre-wrap;">set_tick_hook("MyGame.Player", "LateUpdate")
local id = every(0.016, function()
  if xr.get_bool_feature("grip_button", "right") then
    print("grip")
  end
end)
cancel(id)
cancel_all()</pre>

        <h3 style="color:var(--type);margin:16px 0 8px;">Scene</h3>
        <pre style="background:var(--bg);padding:10px;border-radius:var(--rad-md);border:1px solid var(--brd);margin-bottom:12px;color:var(--fg);white-space:pre-wrap;">local objs = scene.get_objects()
local found = scene.find_object("Player")
local list = scene.find_objects_of_type("UnityEngine.Rigidbody")
local same = wrap(found.addr)</pre>

      </div>
    </div>
  </div>
</div>

<div class="modal-overlay" id="addCompModal" onclick="close_add_comp(event)">
  <div class="modal-box" onclick="event.stopPropagation()">
    <div class="modal-hdr">
      <span>Add Component</span>
      <button class="btn btn-red" style="padding:2px 8px;" onclick="close_add_comp()"><span class="g-icon">close</span></button>
    </div>
    <div style="padding:8px 10px;border-bottom:1px solid var(--brd);"><input class="inp" id="addCompSearch" placeholder="Search component type..." oninput="filter_add_comp()"></div>
    <div class="modal-body" id="addCompCats">Loading...</div>
  </div>
</div>

<div class="modal-overlay" id="collModal" onclick="close_collection(event)">
  <div class="modal-box" onclick="event.stopPropagation()">
    <div class="modal-hdr">
      <span id="collTitle">Collection</span>
      <button class="btn btn-red" style="padding:2px 8px;" onclick="close_collection()"><span class="g-icon">close</span></button>
    </div>
    <div class="modal-body" id="collList"></div>
  </div>
</div>

<div class="modal-overlay" id="instPickerModal" onclick="close_instance_picker(event)">
  <div class="modal-box" onclick="event.stopPropagation()">
    <div class="modal-hdr">
      <span id="instPickerTitle">Select Instance</span>
      <button class="btn btn-red" style="padding:2px 8px;" onclick="close_instance_picker()"><span class="g-icon">close</span></button>
    </div>
    <div style="padding:8px 10px;border-bottom:1px solid var(--brd);display:flex;flex-direction:column;gap:6px;">
      <input class="inp" id="instPickerSearch" placeholder="Search by name or address..." oninput="filter_instance_picker()">
      <div style="display:flex;gap:6px;">
        <button class="btn btn-blu" style="flex:1;" onclick="pick_instance('0x0', 'null (0x0)')"><span class="g-icon">block</span> Null (0x0)</button>
        <button class="btn btn-grn" style="flex:1;" id="btn-pick-selected-go" onclick="pick_current_selected()"><span class="g-icon">deployed_code</span> Current Object</button>
      </div>
    </div>
    <div class="modal-body" id="instPickerList">Loading instances...</div>
  </div>
</div>

<svg style="display:none">
  <symbol id="ic-asm" viewBox="0 0 24 24"><path d="M4 6h16v2H4zm0 5h16v2H4zm0 5h16v2H4z"/></symbol>
  <symbol id="ic-ns" viewBox="0 0 24 24"><path d="M10 4H4c-1.1 0-2 .9-2 2v12c0 1.1.9 2 2 2h16c1.1 0 2-.9 2-2V8c0-1.1-.9-2-2-2h-8z"/></symbol>
  <symbol id="ic-cls" viewBox="0 0 24 24"><path d="M12 2L2 7l10 5 10-5zm0 10.5l-10-5v5.1l10 5 10-5v-5.1z"/></symbol>
  <symbol id="ic-go" viewBox="0 0 24 24"><path d="M12 2L2 7l10 5 10-5zm0 10.5l-10-5v5.1l10 5 10-5v-5.1zM2 17l10 5 10-5"/></symbol>
  <symbol id="ic-comp" viewBox="0 0 24 24"><path d="M14 2H6c-1.1 0-2 .9-2 2v16c0 1.1.9 2 2 2h12c1.1 0 2-.9 2-2V8l-6-6zm2 16H8v-2h8v2zm0-4H8v-2h8v2zm-3-5V3.5L18.5 9H13z"/></symbol>
  <symbol id="ic-play" viewBox="0 0 24 24"><path d="M8 5v14l11-7z"/></symbol>
  <symbol id="ic-loop" viewBox="0 0 24 24"><path d="M12 4V1L8 5l4 4V6c3.31 0 6 2.69 6 6 0 1.01-.25 1.97-.7 2.8l1.46 1.46A7.93 7.93 0 0 0 20 12c0-4.42-3.58-8-8-8zm0 14c-3.31 0-6-2.69-6-6 0-1.01.25-1.97.7-2.8L5.24 7.74A7.93 7.93 0 0 0 4 12c0 4.42 3.58 8 8 8v3l4-4-4-4v3z"/></symbol>
  <symbol id="ic-script" viewBox="0 0 24 24"><path d="M9.4 16.6L4.8 12l4.6-4.6L8 6l-6 6 6 6 1.4-1.4zm5.2 0l4.6-4.6-4.6-4.6L16 6l6 6-6 6-1.4-1.4z"/></symbol>
  <symbol id="ic-del" viewBox="0 0 24 24"><path d="M6 19c0 1.1.9 2 2 2h8c1.1 0 2-.9 2-2V7H6v12zM19 4h-3.5l-1-1h-5l-1 1H5v2h14V4z"/></symbol>
  <symbol id="ic-add" viewBox="0 0 24 24"><path d="M19 3H5c-1.1 0-2 .9-2 2v14c0 1.1.9 2 2 2h14c1.1 0 2-.9 2-2V5c0-1.1-.9-2-2-2zm-2 10h-4v4h-2v-4H7v-2h4V7h2v4h4v2z"/></symbol>
  <symbol id="ic-catch" viewBox="0 0 24 24"><path d="M12 2C6.48 2 2 6.48 2 12s4.48 10 10 10 10-4.48 10-10S17.52 2 12 2zm0 18c-4.42 0-8-3.58-8-8s3.58-8 8-8 8 3.58 8 8-3.58 8-8 8zm3.5-9c.83 0 1.5-.67 1.5-1.5S16.33 8 15.5 8 14 8.67 14 9.5s.67 1.5 1.5 1.5zm-7 0c.83 0 1.5-.67 1.5-1.5S9.33 8 8.5 8 7 8.67 7 9.5 7.67 11 8.5 11zm3.5 6.5c2.33 0 4.31-1.46 5.11-3.5H6.89c.8 2.04 2.78 3.5 5.11 3.5z"/></symbol>
  <symbol id="ic-save" viewBox="0 0 24 24"><path d="M17 3H5a2 2 0 0 0-2 2v14a2 2 0 0 0 2 2h14c1.1 0 2-.9 2-2V7l-4-4zm-5 16c-1.66 0-3-1.34-3-3s1.34-3 3-3 3 1.34 3 3-1.34 3-3 3zm3-10H5V5h10v4z"/></symbol>
</svg>

<script>
let cur_asm="",cur_ns="",cur_cls="";
let all_cls_cache=null;
let full_scene_data=[];
let scene_roots=[];
let selected_go_addr=null;
let selected_go_name="";
let open_scene_nodes=new Set();
let live_sync_active=false;
let live_sync_timer=null;
let add_comp_go_addr=null;
let add_comp_data={};
let picker_target_input_id=null;
let picker_raw_instances=[];

function esc(s){
  if(!s)return"";
  return String(s).replace(/&/g,"&amp;").replace(/</g,"&lt;").replace(/>/g,"&gt;").replace(/"/g,"&quot;").replace(/'/g,"&#039;");
}

function uid8(){return Math.random().toString(36).substr(2,8);}

function toggle_sidebar(open){
  let sb=document.getElementById('app-sidebar');
  let ov=document.getElementById('sidebar-overlay');
  if(open===undefined)open=!sb.classList.contains('open');
  if(open){sb.classList.add('open');ov.classList.add('open');}
  else{sb.classList.remove('open');ov.classList.remove('open');}
}

async function init(){
  try{
    let asms=await fetch('/api/assemblies').then(r=>r.json());
    document.getElementById('tree-root').innerHTML=asms.map(a=>
      `<ul class="tree-node"><div class="tree-nc" onclick="toggle_asm(this,'${esc(a)}')"><span class="arr">&#9654;</span><div class="ico" style="color:#fbbf24"><span class="g-icon">inventory_2</span></div><span>${esc(a)}</span></div><div class="kids"></div></ul>`
    ).join('');
  }catch(e){}
  load_scene();
}

async function toggle_asm(el,asm){
  let kids=el.nextElementSibling,arr=el.querySelector('.arr');
  if(kids.classList.contains('open')){kids.classList.remove('open');arr.classList.remove('open');return;}
  if(!kids.innerHTML.trim()){
    let classes=await fetch(`/api/classes?a=${encodeURIComponent(asm)}`).then(r=>r.json());
    let by_ns={};
    for(let c of classes){let ns=c.ns||'';if(!by_ns[ns])by_ns[ns]=[];by_ns[ns].push(c);}
    let h='';
    for(let ns of Object.keys(by_ns).sort()){
      h+=`<ul class="tree-node"><div class="tree-nc" onclick="toggle_ns(this)"><span class="arr">&#9654;</span><div class="ico" style="color:#f59e0b"><span class="g-icon">folder</span></div><span>${esc(ns||'(global)')}</span></div><div class="kids">`;
      for(let c of by_ns[ns].sort((a,b)=>a.name.localeCompare(b.name))){
        let badge=c.t!=='class'?`<span class="mono-tag">${c.t}</span>`:'';
        h+=`<div class="tree-nc" onclick="load_cls(this,'${esc(asm)}','${esc(c.ns||'')}','${esc(c.name)}')"><span class="arr"></span><div class="ico" style="color:var(--type)"><span class="g-icon">category</span></div><span>${esc(c.name)}</span>${badge}</div>`;
      }
      h+=`</div></ul>`;
    }
    kids.innerHTML=h;
  }
  kids.classList.add('open');arr.classList.add('open');
}

function toggle_ns(el){el.nextElementSibling.classList.toggle('open');el.querySelector('.arr').classList.toggle('open');}

function format_code(d){
  let ns=d.ns||'',name=d.name||'',parent=d.parent||'';
  let t=d.type||'class';
  let ind=ns?'    ':'';
  let lines=[];
  if(ns) lines.push(`<span class="c-kw">namespace</span> <span class="c-ty">${esc(ns)}</span> {`);
  let decl=`${ind}<span class="c-kw">public ${t}</span> <span class="c-ty">${esc(name)}</span>`;
  if(parent&&parent!=='Object'&&parent!=='ValueType') decl+=` : <span class="c-ty">${esc(parent)}</span>`;
  lines.push(decl+' {');
  for(let f of d.fields||[]){
    let is_static=f.s?' <span class="c-kw">static</span>':'';
    lines.push(`${ind}    <span class="c-kw">public</span>${is_static} <span class="c-ty">${esc(f.type)}</span> <span>${esc(f.name)}</span>; <span class="c-cm">// 0x${f.off?Number(f.off).toString(16):'?'}</span>`);
  }
  for(let p of d.props||[]){
    let acc=(p.g?'get; ':'')+(p.s?'set; ':'');
    lines.push(`${ind}    <span class="c-kw">public</span> <span class="c-ty">${esc(p.type)}</span> <span class="c-mt">${esc(p.name)}</span> { ${acc}}`);
  }
  for(let m of d.methods||[]){
    let prms=(m.params||[]).map(p=>`<span class="c-ty">${esc(p.t)}</span> ${esc(p.n)}`).join(', ');
    let is_static=m.s?' <span class="c-kw">static</span>':'';
    let addr=m.addr?` <span class="c-cm">// ${m.addr}</span>`:'';
    lines.push(`${ind}    <span class="c-kw">public</span>${is_static} <span class="c-ty">${esc(m.ret)}</span> <span class="c-mt">${esc(m.name)}</span>(${prms}) {}${addr}`);
  }
  lines.push(ind+'}');
  if(ns) lines.push('}');
  return lines.join('\n');
}

async function load_cls(el,asm,ns,name){
  document.querySelectorAll('#tree-root .tree-nc').forEach(e=>e.classList.remove('active'));
  if(el)el.classList.add('active');
  cur_asm=asm;cur_ns=ns;cur_cls=name;
  toggle_sidebar(false);
  let d=await fetch(`/api/class?a=${encodeURIComponent(asm)}&ns=${encodeURIComponent(ns)}&n=${encodeURIComponent(name)}`).then(r=>r.json());
  document.getElementById('pnl-code').innerHTML=format_code(d);
  switch_tab('code');
  load_instances();
}

async function load_instances(){
  document.getElementById('inst-list').innerHTML='Loading...';
  let d=await fetch(`/api/instances?a=${encodeURIComponent(cur_asm)}&ns=${encodeURIComponent(cur_ns)}&n=${encodeURIComponent(cur_cls)}`).then(r=>r.json());
  if(d.error){document.getElementById('inst-list').innerHTML=`<div style="color:#f87171;padding:12px;">${esc(d.error)}</div>`;return;}
  let h=`<div style="display:flex;justify-content:space-between;align-items:center;margin-bottom:8px;"><span>Found instances: ${d.instances.length}</span><button class="btn btn-blu" onclick="open_static_controller()"><span class="g-icon">gamepad</span> Open Static</button></div><table class="table"><tr><th>Address</th><th>Name</th><th>Action</th></tr>`;
  for(let i of d.instances)
    h+=`<tr><td><span class="c-nm">0x${i.addr}</span></td><td><span class="c-st">"${esc(i.name)}"</span></td><td><button class="btn btn-blu" onclick="open_controller('0x${i.addr}')"><span class="g-icon">touch_app</span> Open</button></td></tr>`;
  document.getElementById('inst-list').innerHTML=h+'</table>';
}

function open_controller(addr){
  document.getElementById('ctrl-addr').value=addr;
  document.getElementById('ctrl-asm').value=cur_asm;
  document.getElementById('ctrl-ns').value=cur_ns;
  document.getElementById('ctrl-cls').value=cur_cls;
  document.getElementById('ctrl-static').checked=false;
  document.getElementById('ctrl-addr').disabled=false;
  switch_tab('ctrl');load_controller();
}

function open_static_controller(){
  document.getElementById('ctrl-addr').value='0x0';
  document.getElementById('ctrl-asm').value=cur_asm;
  document.getElementById('ctrl-ns').value=cur_ns;
  document.getElementById('ctrl-cls').value=cur_cls;
  document.getElementById('ctrl-static').checked=true;
  document.getElementById('ctrl-addr').disabled=true;
  switch_tab('ctrl');load_controller();
}

function open_ctrl_instance_picker(){
  let cls=document.getElementById('ctrl-cls').value||'';
  let ns=document.getElementById('ctrl-ns').value||'';
  let full=ns?(ns+'.'+cls):cls;
  open_instance_picker(full||'UnityEngine.GameObject','ctrl-addr');
}

async function load_controller(){
  let addr=document.getElementById('ctrl-addr').value;
  let asm=document.getElementById('ctrl-asm').value;
  let ns=document.getElementById('ctrl-ns').value;
  let cls=document.getElementById('ctrl-cls').value;
  document.getElementById('ctrl-insp').innerHTML='<div style="padding:30px;text-align:center;">Loading target data...</div>';
  let d=await fetch(`/api/controller/inspect?addr=${addr}&asm=${encodeURIComponent(asm)}&ns=${encodeURIComponent(ns)}&cls=${encodeURIComponent(cls)}`).then(r=>r.json());
  if(!d.name){document.getElementById('ctrl-insp').innerHTML='<div style="padding:30px;color:#f87171;text-align:center;">Target class or instance not found</div>';return;}
  let h=`<div class="insp-hdr"><div class="ico" style="color:var(--type)"><span class="g-icon">category</span></div><span class="inp">${esc(d.name)} (${addr})</span></div>`;
  if(d.fields?.length) h+=build_comp_ui("Fields",false,d.fields,addr,'instance',true,false);
  if(d.methods?.length){
    let uid=uid8();
    h+=`<div class="comp"><div class="comp-hdr" onclick="toggle_comp('${uid}')"><div class="ico"><span class="g-icon">play_arrow</span></div><span>Methods (${d.methods.length})</span></div><div class="comp-body open" id="${uid}">`;
    d.methods.forEach((m,idx)=>{
      h+=`<div style="padding:8px 12px;border-bottom:1px solid var(--brd);background:var(--bg2);">`;
      h+=`<div style="display:flex;align-items:center;justify-content:space-between;margin-bottom:6px;">`;
      h+=`<span style="color:var(--mth);font-weight:700;font-size:12px;">${m.s?'[Static] ':''}${esc(m.name)}</span>`;
      h+=`<div style="display:flex;gap:6px;align-items:center;">`;
      h+=`<span style="color:var(--type);font-size:10px;font-weight:600;">${esc(m.ret)}</span>`;
      h+=`<button class="btn btn-blu" style="padding:2px 7px;" title="Add to Catcher" onclick="catch_method('${esc(asm)}','${esc(ns)}','${esc(cls)}','${esc(m.name)}',this)"><span class="g-icon">track_changes</span></button>`;
      h+=`</div></div>`;
      if(m.params?.length){
        h+=`<div style="padding:6px 8px;background:var(--bg);border-radius:var(--rad-md);border:1px solid var(--brd);margin-bottom:6px;">`;
        m.params.forEach((p,pidx)=>{
          let prim=['System.Int32','Int32','int','System.Single','Single','float','System.Double','Double','double','System.Boolean','Boolean','bool','System.String','String','string','System.Int64','Int64','long','System.UInt32','UInt32','uint','System.Byte','Byte','byte','System.Int16','Int16','short','UnityEngine.Vector3','Vector3','UnityEngine.Color','Color','UnityEngine.Vector2','Vector2'];
          let is_obj=!prim.includes(p.t);
          let ph=is_obj?'0x... (address)':esc(p.n);
          let input_id=`marg-input-${idx}-${pidx}`;
          h+=`<div style="display:flex;gap:6px;margin-bottom:4px;align-items:center;"><span style="width:110px;color:var(--fg-dim);font-size:10px;overflow:hidden;text-overflow:ellipsis;" title="${esc(p.t)}">${esc(p.t)}</span>`;
          h+=`<input type="text" id="${input_id}" class="inp marg-${idx}" data-type="${esc(p.t)}" placeholder="${ph}" style="flex:1;">`;
          if(is_obj){
            h+=`<button class="btn btn-blu" style="padding:4px 8px;" onclick="open_instance_picker('${esc(p.t)}','${input_id}')" title="Select Live Instance"><span class="g-icon">touch_app</span> Select</button>`;
          }
          h+=`</div>`;
        });
        h+=`</div>`;
      }
      h+=`<div style="display:flex;align-items:center;gap:6px;"><button class="btn btn-grn" onclick="exec_method('${esc(asm)}','${esc(ns)}','${esc(cls)}','${esc(m.name)}',${m.s},'${addr}',${idx})"><span class="g-icon">play_arrow</span> Run</button><div class="ctrl-res" id="mres-${idx}"></div></div></div>`;
    });
    h+=`</div></div>`;
  }
  document.getElementById('ctrl-insp').innerHTML=h;
}

async function exec_method(asm,ns,cls,m_name,is_static,addr,idx){
  let args=Array.from(document.querySelectorAll(`.marg-${idx}`)).map(el=>({t:el.getAttribute('data-type'),v:el.value}));
  let res=document.getElementById(`mres-${idx}`);
  res.innerHTML='<span style="color:#888;">Invoking...</span>';
  let d=await fetch('/api/invoke',{method:'POST',headers:{'Content-Type':'application/json'},body:JSON.stringify({asm,ns,cls,method:m_name,static:is_static,instance:addr,args})}).then(r=>r.json());
  let rm=d.ok?String(d.value).match(/ @ 0x([0-9A-Fa-f]+)/):null;
  let open_btn=rm&&!/^0+$/.test(rm[1])?` <button class="btn" style="padding:1px 6px;" onclick="open_object_ref('${rm[1]}')"><span class="g-icon">open_in_new</span></button>`:'';
  res.innerHTML=d.ok?`<span style="color:var(--type);font-weight:600;">${esc(d.value)}</span>${open_btn}`:`<span style="color:#f87171;">${esc(d.error)}</span>`;
}

async function catch_method(asm,ns,cls,method,btn){
  btn.style.background='#3a3a3a';
  let d=await fetch('/api/catcher/hook',{method:'POST',headers:{'Content-Type':'application/json'},body:JSON.stringify({asm,ns,cls,method})}).then(r=>r.json());
  if(d.ok){btn.style.background='#3d3d3d';refresh_catcher();}
  else{btn.style.background='var(--red)';alert('Hook failed: '+(d.error||'unknown'));}
}

function switch_tab(id){
  document.querySelectorAll('.tab').forEach(t=>t.classList.remove('active'));
  document.querySelectorAll('.panel').forEach(p=>p.classList.remove('active'));
  document.getElementById(`tab-${id}`).classList.add('active');
  document.getElementById(`pnl-${id}`).classList.add('active');
  if(id==='loops')refresh_loops_ui();
  if(id==='catcher')refresh_catcher();
}

async function do_search(e){
  let q=document.getElementById('gSearch').value.toLowerCase();
  if(!q){await init();return;}
  if(e.key!=='Enter')return;
  if(!all_cls_cache)all_cls_cache=await fetch('/api/allclasses').then(r=>r.json());
  let res=all_cls_cache.filter(c=>c.name.toLowerCase().includes(q)||c.ns?.toLowerCase().includes(q));
  let by_asm={};
  for(let c of res){if(!by_asm[c.a])by_asm[c.a]=[];by_asm[c.a].push(c);}
  let html='';
  for(let a of Object.keys(by_asm).sort()){
    html+=`<ul class="tree-node"><div class="tree-nc" onclick="toggle_asm(this,'${esc(a)}')"><span class="arr open">&#9654;</span><div class="ico" style="color:#fbbf24"><span class="g-icon">inventory_2</span></div><span>${esc(a)}</span></div><div class="kids open">`;
    let by_ns={};
    for(let c of by_asm[a]){let ns=c.ns||'';if(!by_ns[ns])by_ns[ns]=[];by_ns[ns].push(c);}
    for(let ns of Object.keys(by_ns).sort()){
      html+=`<ul class="tree-node"><div class="tree-nc" onclick="toggle_ns(this)"><span class="arr open">&#9654;</span><div class="ico" style="color:#f59e0b"><span class="g-icon">folder</span></div><span>${esc(ns||'(global)')}</span></div><div class="kids open">`;
      for(let c of by_ns[ns].sort((x,y)=>x.name.localeCompare(y.name))){
        html+=`<div class="tree-nc" onclick="load_cls(this,'${esc(a)}','${esc(c.ns||'')}','${esc(c.name)}')"><span class="arr"></span><div class="ico" style="color:var(--type)"><span class="g-icon">category</span></div><span>${esc(c.name)}</span></div>`;
      }
      html+=`</div></ul>`;
    }
    html+=`</div></ul>`;
  }
  document.getElementById('tree-root').innerHTML=html;
}

async function load_scene(manual_reload){
  try{
    full_scene_data=await fetch('/api/scene').then(r=>r.json());
    filter_scene();
    if(selected_go_addr){
      let current=full_scene_data.find(g=>g.addr===selected_go_addr);
      if(!current&&manual_reload){
        document.getElementById('scene-insp').innerHTML='<div style="padding:30px;text-align:center;color:var(--fg-dim);">Selected GameObject no longer exists</div>';
        selected_go_addr=null;
      }
    }
  }catch(e){}
}

function toggle_live_sync(){
  live_sync_active=!live_sync_active;
  let btn=document.getElementById('btn-live-sync');
  let dot=document.getElementById('live-dot');
  if(live_sync_active){
    btn.classList.add('active');
    dot.classList.remove('off');
    live_sync_timer=setInterval(async()=>{
      await load_scene(false);
      if(selected_go_addr){
        let d=await fetch(`/api/scene/inspect?addr=${selected_go_addr}`).then(r=>r.json()).catch(()=>null);
        if(d&&d.transform&&!d.stale){
          update_transform_inputs_live(d.transform);
        }
      }
    },2500);
  }else{
    btn.classList.remove('active');
    dot.classList.add('off');
    if(live_sync_timer){clearInterval(live_sync_timer);live_sync_timer=null;}
  }
}

function update_transform_inputs_live(t){
  if(!t)return;
  let pinp=document.querySelectorAll('[data-tt="transform"][data-pid="p"]');
  if(pinp.length===3&&t.p){t.p.forEach((v,i)=>{if(document.activeElement!==pinp[i])pinp[i].value=v;});}
  let rinp=document.querySelectorAll('[data-tt="transform"][data-pid="r"]');
  if(rinp.length===3&&t.r){t.r.forEach((v,i)=>{if(document.activeElement!==rinp[i])rinp[i].value=v;});}
  let sinp=document.querySelectorAll('[data-tt="transform"][data-pid="s"]');
  if(sinp.length===3&&t.s){t.s.forEach((v,i)=>{if(document.activeElement!==sinp[i])sinp[i].value=v;});}
}

function filter_scene(){
  let q=document.getElementById('sceneSearch').value.toLowerCase();
  let map={};scene_roots=[];
  for(let g of full_scene_data){
    if(!map[g.addr])map[g.addr]={...g,children:[]};
    else Object.assign(map[g.addr],g);
    if(!g.parent||g.parent==="0"||g.parent===0)scene_roots.push(map[g.addr]);
    else{if(!map[g.parent])map[g.parent]={children:[]};map[g.parent].children.push(map[g.addr]);}
  }
  document.getElementById('scene-tree').innerHTML=scene_roots.map(n=>build_scene_node(n,q)).filter(Boolean).join('');
  if(selected_go_addr){
    let el=document.querySelector(`[data-go="${selected_go_addr}"]`);
    if(el)el.classList.add('active');
  }
}

function toggle_scene_expand(addr,el){
  if(open_scene_nodes.has(addr))open_scene_nodes.delete(addr);
  else open_scene_nodes.add(addr);
  let kids=el.parentElement.nextElementSibling;
  let arr=el.querySelector('.arr');
  if(kids){kids.classList.toggle('open');arr.classList.toggle('open');}
}

function build_scene_node(n,q){
  let matches=!q||n.name?.toLowerCase().includes(q);
  let child_html=(n.children||[]).map(c=>build_scene_node(c,q)).filter(Boolean).join('');
  if(!matches&&!child_html)return'';
  let has_kids=(n.children||[]).length>0;
  let is_open=open_scene_nodes.has(n.addr)||(q&&matches);
  return`<ul class="tree-node"><div class="tree-nc" data-go="${n.addr}" onclick="inspect_go('${n.addr}')"><span class="arr${is_open?' open':''}" onclick="event.stopPropagation();toggle_scene_expand('${n.addr}',this)">${has_kids?'&#9654;':''}</span><div class="ico"><span class="g-icon" style="font-size:15px;">deployed_code</span></div><span style="color:${n.active?'var(--fg)':'#7a7a7a'}">${esc(n.name)}</span></div>${has_kids?`<div class="kids${is_open?' open':''}">${child_html}</div>`:''}</ul>`;
}

async function inspect_go(addr){
  document.querySelectorAll('#scene-tree .tree-nc').forEach(e=>e.classList.remove('active'));
  let el=document.querySelector(`[data-go="${addr}"]`);
  if(el)el.classList.add('active');
  selected_go_addr=addr;
  let d=await fetch(`/api/scene/inspect?addr=${addr}`).then(r=>r.json());
  if(d.stale||!d.addr){
    document.getElementById('scene-insp').innerHTML='<div style="padding:30px;text-align:center;color:#f87171;">Object is no longer active in scene</div>';
    selected_go_addr=null;
    return;
  }
  selected_go_name=d.name||"GameObject";
  let h=`<div class="insp-hdr">
    <input type="checkbox" ${d.active?'checked':''} onchange="update_go('${d.addr}','gameobject','active',this.checked)">
    <div class="ico" style="color:var(--type)"><span class="g-icon">deployed_code</span></div>
    <input type="text" class="inp" value="${esc(d.name)}" onchange="update_go('${d.addr}','gameobject','name',this.value)">
    <button class="btn btn-grn" style="padding:4px 8px;" onclick="open_add_comp('${d.addr}')" title="Add Component"><span class="g-icon">add</span> Add</button>
    <button class="btn btn-red" style="padding:4px 8px;" onclick="delete_go('${d.addr}')" title="Delete GameObject"><span class="g-icon">delete</span></button>
  </div>`;
  if(d.transform)
    h+=build_comp_ui("Transform",true,[
      {lbl:"Position",id:"p",val:d.transform.p,type:"UnityEngine.Vector3",canWrite:true},
      {lbl:"Rotation",id:"r",val:d.transform.r,type:"UnityEngine.Vector3",canWrite:true},
      {lbl:"Scale",id:"s",val:d.transform.s,type:"UnityEngine.Vector3",canWrite:true}
    ],d.transform.addr,'transform',true);
  for(let sc of d.scripts||[])h+=build_script_comp_ui(sc,addr);
  document.getElementById('scene-insp').innerHTML=h;
}

async function delete_go(addr){
  if(!confirm('Delete this GameObject?'))return;
  await fetch('/api/scene/delete',{method:'POST',headers:{'Content-Type':'application/json'},body:JSON.stringify({addr})});
  document.getElementById('scene-insp').innerHTML='<div style="padding:30px;text-align:center;color:var(--fg-dim);">Deleted</div>';
  selected_go_addr=null;
  await load_scene(true);
}

async function open_add_comp(go_addr){
  add_comp_go_addr=go_addr;
  document.getElementById('addCompSearch').value='';
  document.getElementById('addCompCats').innerHTML='Loading categories...';
  document.getElementById('addCompModal').classList.add('open');
  if(!Object.keys(add_comp_data).length){
    add_comp_data=await fetch('/api/scene/addable').then(r=>r.json());
  }
  render_add_comp_cats('');
}

function render_add_comp_cats(q){
  let h='';
  for(let [cat,items] of Object.entries(add_comp_data)){
    let filtered=q?items.filter(i=>i.name.toLowerCase().includes(q)||i.full.toLowerCase().includes(q)):items;
    if(!filtered.length)continue;
    let uid=uid8();
    h+=`<div class="add-comp-cat-hdr" onclick="document.getElementById('cat-${uid}').classList.toggle('open')">${esc(cat)} <span style="opacity:.6;font-size:10px;">(${filtered.length})</span></div>`;
    h+=`<div class="add-comp-items${q?' open':''}" id="cat-${uid}">`;
    for(let item of filtered)
      h+=`<div class="add-comp-item" onclick="add_component('${esc(item.full)}')">${esc(item.name)}<span style="color:var(--fg-dim);margin-left:6px;font-size:10px;">${esc(item.full)}</span></div>`;
    h+=`</div>`;
  }
  document.getElementById('addCompCats').innerHTML=h||'<div style="padding:16px;color:var(--fg-dim);text-align:center;">No matching components found</div>';
}

function filter_add_comp(){render_add_comp_cats(document.getElementById('addCompSearch').value.toLowerCase());}

function close_add_comp(e){
  if(!e||e.target===document.getElementById('addCompModal'))
    document.getElementById('addCompModal').classList.remove('open');
}

async function add_component(full_type){
  document.getElementById('addCompModal').classList.remove('open');
  let d=await fetch('/api/scene/addcomponent',{method:'POST',headers:{'Content-Type':'application/json'},body:JSON.stringify({addr:add_comp_go_addr,type:full_type})}).then(r=>r.json());
  if(d.ok){
    inspect_go(add_comp_go_addr);
  } else alert('Failed: '+(d.error||'unknown'));
}

async function remove_component(comp_addr,go_addr){
  if(!confirm('Remove this component?'))return;
  await fetch('/api/scene/removecomponent',{method:'POST',headers:{'Content-Type':'application/json'},body:JSON.stringify({addr:comp_addr})});
  inspect_go(go_addr);
}

function parse_obj_ref(v){
  let t=String(v==null?'':v).replace(/^"|"$/g,'');
  let m=t.match(/^0x([0-9A-Fa-f]+)/);
  if(!m||/^0+$/.test(m[1]))return null;
  return {addr:m[1],is_list:/\[\d+\]\)\s*$/.test(t)};
}

async function open_object_ref(raw){
  let addr=String(raw).replace(/^0x/i,'').split(' ')[0];
  if(!addr||/^0+$/.test(addr))return;
  let d=await fetch(`/api/object/info?addr=${addr}`).then(r=>r.json()).catch(()=>null);
  if(!d||!d.ok){alert('Object is no longer valid');return;}
  if(d.kind==='gameobject'||d.kind==='component'){
    if(!d.go){alert('GameObject not found');return;}
    close_collection();
    switch_tab('scene');
    inspect_go(d.go);
    return;
  }
  if(d.kind==='collection'||d.kind==='counted'){open_collection(addr);return;}
  close_collection();
  cur_asm=d.asm;cur_ns=d.ns;cur_cls=d.cls;
  open_controller('0x'+addr);
}

async function open_collection(addr){
  document.getElementById('collTitle').innerText='Collection 0x'+addr;
  document.getElementById('collList').innerHTML='<div style="padding:20px;text-align:center;color:var(--fg-dim);">Loading...</div>';
  document.getElementById('collModal').classList.add('open');
  let d=await fetch(`/api/collection?addr=${addr}`).then(r=>r.json()).catch(()=>null);
  if(!d||!d.ok){
    document.getElementById('collList').innerHTML=`<div style="padding:20px;text-align:center;color:#f87171;">${esc(d&&d.error?d.error:'Failed to read collection')}</div>`;
    return;
  }
  document.getElementById('collTitle').innerText=`${d.kind} (${d.count})${d.elem?' of '+d.elem:''}`;
  if(!d.items.length){
    document.getElementById('collList').innerHTML=`<div style="padding:20px;text-align:center;color:var(--fg-dim);">${d.count>0?'Items are not listable for this collection':'Empty'}</div>`;
    return;
  }
  let h='';
  for(let it of d.items){
    let clk=it.addr?` clk" onclick="open_object_ref('${it.addr}')`:'';
    h+=`<div class="coll-row${clk}"><span class="coll-idx">${esc(it.i)}</span><span class="coll-val" title="${esc(it.v)}">${esc(it.v)}</span><span class="coll-ty">${esc(it.t)}</span>${it.addr?'<span class="g-icon">chevron_right</span>':''}</div>`;
  }
  if(d.truncated)h+='<div style="padding:10px;text-align:center;color:var(--fg-dim);">Showing first 300 items</div>';
  document.getElementById('collList').innerHTML=h;
}

function close_collection(e){
  if(!e||e.target===document.getElementById('collModal'))
    document.getElementById('collModal').classList.remove('open');
}

function open_instance_picker(type_name,target_input_id){
  picker_target_input_id=target_input_id;
  document.getElementById('instPickerTitle').innerText='Select Instance: '+type_name;
  document.getElementById('instPickerSearch').value='';
  document.getElementById('instPickerList').innerHTML='<div style="padding:20px;text-align:center;color:var(--fg-dim);">Loading instances from game...</div>';
  document.getElementById('instPickerModal').classList.add('open');
  let btn_cur=document.getElementById('btn-pick-selected-go');
  if(selected_go_addr){
    btn_cur.style.display='inline-flex';
    btn_cur.innerText='Current ('+selected_go_name+')';
  }else{
    btn_cur.style.display='none';
  }
  fetch(`/api/find_instances?type=${encodeURIComponent(type_name)}`).then(r=>r.json()).then(d=>{
    picker_raw_instances=d.instances||[];
    render_instance_picker('');
  }).catch(()=>{
    document.getElementById('instPickerList').innerHTML='<div style="padding:20px;text-align:center;color:#f87171;">Failed to query instances</div>';
  });
}

function render_instance_picker(q){
  let filtered=picker_raw_instances;
  if(q){
    filtered=filtered.filter(i=>(i.name&&i.name.toLowerCase().includes(q))||(i.addr&&i.addr.toLowerCase().includes(q)));
  }
  if(!filtered.length){
    document.getElementById('instPickerList').innerHTML='<div style="padding:20px;text-align:center;color:var(--fg-dim);">No instances found in scene memory</div>';
    return;
  }
  let h='';
  for(let i of filtered){
    let addr_hex=i.addr.startsWith('0x')?i.addr:('0x'+i.addr);
    h+=`<div class="inst-item" onclick="pick_instance('${addr_hex}','${esc(i.name)}')">
      <div style="display:flex;flex-direction:column;gap:2px;overflow:hidden;">
        <span style="font-weight:700;color:var(--fg);">${esc(i.name||'Instance')}</span>
        <span style="font-size:10px;color:var(--num);font-family:Consolas,monospace;">${addr_hex}</span>
      </div>
      <button class="btn btn-blu" style="padding:2px 8px;"><span class="g-icon">check</span> Select</button>
    </div>`;
  }
  document.getElementById('instPickerList').innerHTML=h;
}

function filter_instance_picker(){
  render_instance_picker(document.getElementById('instPickerSearch').value.toLowerCase());
}

function pick_instance(addr,name){
  if(picker_target_input_id){
    let input=document.getElementById(picker_target_input_id);
    if(input){
      input.value=addr;
      let evt=new Event('change',{bubbles:true});
      input.dispatchEvent(evt);
    }
  }
  document.getElementById('instPickerModal').classList.remove('open');
}

function pick_current_selected(){
  if(selected_go_addr){
    pick_instance(selected_go_addr.startsWith('0x')?selected_go_addr:('0x'+selected_go_addr),selected_go_name);
  }
}

function close_instance_picker(e){
  if(!e||e.target===document.getElementById('instPickerModal'))
    document.getElementById('instPickerModal').classList.remove('open');
}

function build_script_comp_ui(sc,go_addr){
  let uid=uid8();
  let is_builtin=sc.category==='builtin';
  let hdr_cls=is_builtin?'builtin-hdr':'script-hdr';
  let col=is_builtin?'var(--kw)':'var(--mth)';
  let h=`<div class="comp"><div class="comp-hdr ${hdr_cls}" onclick="toggle_comp('${uid}')">
    <input type="checkbox" ${sc.enabled?'checked':''} onchange="update_script_enabled('${sc.addr}',this.checked)" onclick="event.stopPropagation()" style="margin-right:4px;">
    <div class="ico" style="color:${col}"><span class="g-icon">${is_builtin?'widgets':'terminal'}</span></div>
    <span style="color:${col}">${esc(sc.name)}</span>
    ${sc.ns?`<span style="opacity:.6;font-size:10px;margin-left:4px;">${esc(sc.ns)}</span>`:''}
    <span style="opacity:.4;font-size:10px;margin-left:auto;margin-right:4px;">0x${sc.addr}</span>
    <button class="btn btn-red" style="padding:2px 6px;" title="Remove" onclick="event.stopPropagation();remove_component('${sc.addr}','${go_addr}')"><span class="g-icon">delete</span></button>
  </div><div class="comp-body open" id="${uid}">`;
  if(!sc.fields?.length){
    h+=`<div class="row" style="padding:6px 10px;color:var(--fg-dim);">No readable fields</div>`;
  } else {
    for(let r of sc.fields){
      let da=`data-addr="${sc.addr}" data-tt="script" data-pid="${r.name}" data-vt="${r.type}" data-ip="${r.isProp?'true':'false'}" data-cns="${esc(sc.ns)}" data-ccls="${esc(sc.name)}" data-casm="${esc(sc.asm||'')}"`;
      let dis=r.canWrite===false?'disabled':'';
      h+=build_field_row(r,da,dis,true,true);
    }
  }
  return h+'</div></div>';
}

function build_comp_ui(title,is_trans,rows,target_addr,target_type,with_save,is_script){
  let uid=uid8();
  let h=`<div class="comp"><div class="comp-hdr" onclick="toggle_comp('${uid}')"><div class="ico"><span class="g-icon">widgets</span></div><span>${esc(title)}</span></div><div class="comp-body open" id="${uid}">`;
  for(let r of rows){
    let da=`data-addr="${target_addr}" data-tt="${target_type}" data-pid="${r.id||r.name||r.lbl}" data-vt="${r.type}" data-ip="${r.isProp?'true':'false'}" data-static="${r.static?'true':'false'}"`;
    let dis=r.canWrite===false?'disabled':'';
    h+=build_field_row(r,da,dis,with_save,target_type==='instance');
  }
  return h+'</div></div>';
}

function build_field_row(r,da,dis,with_save,show_loop){
  let label=r.name||r.lbl||r.id||'';
  let prefix=r.static?'<span style="color:#e8a0ff;font-size:10px;font-weight:700;">[Static] </span>':'';
  let h=`<div class="row"><div class="lbl" title="${esc(label)}">${prefix}${esc(label)}</div><div class="val">`;
  let t=r.type||'';
  let row_id='frow-'+uid8();

  if(t==="UnityEngine.Vector3"||t==="Vector3"){
    let v=Array.isArray(r.val)?r.val:try_parse_vec3(r.val);
    ['X','Y','Z'].forEach((l,i)=>{h+=`<div style="flex:1;display:flex;min-width:55px;"><span class="vl">${l}</span><input class="inp vec-input" type="number" step="any" value="${v[i]||0}" ${da} ${dis}></div>`;});
  } else if(t==="UnityEngine.Vector2"||t==="Vector2"){
    let v=Array.isArray(r.val)?r.val:try_parse_vec2(r.val);
    ['X','Y'].forEach((l,i)=>{h+=`<div style="flex:1;display:flex;min-width:55px;"><span class="vl">${l}</span><input class="inp vec-input" type="number" step="any" value="${v[i]||0}" ${da} ${dis}></div>`;});
  } else if(t==="UnityEngine.Quaternion"||t==="Quaternion"||t==="UnityEngine.Vector4"||t==="Vector4"){
    let v=r.val&&typeof r.val==='object'?r.val:{x:0,y:0,z:0,w:1};
    if(typeof r.val==='string'){try{v=JSON.parse(r.val);}catch(e){}}
    ['X','Y','Z','W'].forEach((l,k)=>{h+=`<div style="flex:1;display:flex;min-width:50px;"><span class="vl">${l}</span><input class="inp vec-input" type="number" step="any" value="${v[l.toLowerCase()]||0}" ${da} ${dis}></div>`;});
  } else if(t==="UnityEngine.Color"||t==="Color"){
    let v=Array.isArray(r.val)?r.val:[1,1,1,1];
    if(typeof r.val==='string'){try{v=JSON.parse(r.val);}catch(e){}}
    let hex="#"+((1<<24)+(Math.round(v[0]*255)<<16)+(Math.round(v[1]*255)<<8)+Math.round(v[2]*255)).toString(16).slice(1).padStart(6,'0');
    h+=`<input type="color" class="inp sing-input" value="${hex}" style="padding:0;height:26px;width:44px;border-radius:var(--rad-sm);" ${da} ${dis}>`;
    h+=`<span style="font-size:10px;color:var(--fg-dim);">rgba(${v.map(x=>Math.round(x*255)).join(',')})</span>`;
  } else if(t==="UnityEngine.Rect"||t==="Rect"){
    let v=r.val&&typeof r.val==='object'?r.val:{x:0,y:0,w:0,h:0};
    if(typeof r.val==='string'){try{v=JSON.parse(r.val);}catch(e){}}
    ['x','y','w','h'].forEach(k=>{h+=`<div style="flex:1;display:flex;min-width:45px;"><span class="vl">${k}</span><input class="inp vec-input" type="number" step="any" value="${v[k]||0}" ${da} ${dis}></div>`;});
  } else if(t==="UnityEngine.LayerMask"||t==="LayerMask"){
    h+=`<input class="inp sing-input" type="number" step="1" value="${r.val||0}" ${da} ${dis} style="width:80px;">`;
    h+=`<span style="font-size:10px;color:var(--fg-dim);">Layer ${r.val||0}</span>`;
  } else if(t==="System.Boolean"||t==="Boolean"||t==="bool"){
    h+=`<input type="checkbox" class="sing-input" ${(r.val===true||r.val==="true")?'checked':''} ${da} ${dis} style="width:16px;height:16px;">`;
  } else {
    let is_num=["System.Int32","Int32","int","System.Single","Single","float","System.Double","Double","double","System.Int64","Int64","long","System.UInt32","UInt32","uint","System.UInt64","UInt64","ulong","System.Byte","Byte","byte","System.Int16","Int16","short"].includes(t);
    let sv=String(r.val||'').replace(/^"|"$/g,'');
    let is_obj=!is_num&&t!=="System.String"&&t!=="String"&&t!=="string";
    h+=`<input id="${row_id}" class="inp sing-input" ${is_num?'type="number" step="any"':'type="text"'} value="${esc(sv)}" ${da} ${dis} style="min-width:80px;flex:1;">`;
    let oref=is_obj?parse_obj_ref(r.val):null;
    if(oref){
      h+=`<button class="btn" style="padding:2px 7px;" onclick="open_object_ref('${oref.addr}')" title="${oref.is_list?'Show items':'Open object'}"><span class="g-icon">${oref.is_list?'format_list_bulleted':'open_in_new'}</span></button>`;
    }
    if(is_obj&&!dis){
      h+=`<button class="btn btn-blu" style="padding:2px 7px;" onclick="open_instance_picker('${esc(t)}','${row_id}')" title="Pick live instance"><span class="g-icon">touch_app</span></button>`;
    }
  }

  if(with_save&&!dis){
    h+=`<button class="btn btn-grn" style="margin-left:4px;padding:3px 8px;" onclick="handle_save(this)"><span class="g-icon">check</span> Set</button>`;
    if(show_loop)h+=`<button class="btn btn-org" style="padding:3px 7px;" title="Loop" onclick="prompt_loop(this)"><span class="g-icon">sync</span></button>`;
  }
  h+=`</div></div>`;
  return h;
}

function try_parse_vec3(v){try{let a=JSON.parse(v);if(Array.isArray(a))return a;}catch(e){}return[0,0,0];}
function try_parse_vec2(v){try{let a=JSON.parse(v);if(Array.isArray(a))return a;}catch(e){}return[0,0];}

function toggle_comp(id){document.getElementById(id).classList.toggle('open');}

async function update_script_enabled(addr,val){
  await fetch('/api/scene/update',{method:'POST',headers:{'Content-Type':'application/json'},body:JSON.stringify({addr,type:'script',prop:'enabled',val:String(val)})});
}

async function update_go(addr,type,prop,val){
  await fetch('/api/scene/update',{method:'POST',headers:{'Content-Type':'application/json'},body:JSON.stringify({addr,type,prop,val:String(val)})}).catch(()=>({}));
}

function handle_save(btn){
  let el=btn.parentElement.querySelector('.vec-input,.sing-input');
  if(el)process_save(el);
}

function get_val_from_row(el){
  if(el.classList.contains('vec-input')){
    let ins=el.parentElement.parentElement.querySelectorAll('input.vec-input');
    let vals=Array.from(ins).map(i=>i.value);
    let vt=el.getAttribute('data-vt')||'';
    if(vt.includes('Quaternion')||vt.includes('Vector4')){
      return`{"x":${vals[0]},"y":${vals[1]},"z":${vals[2]},"w":${vals[3]||1}}`;
    }
    if(vt.includes('Rect'))return`{"x":${vals[0]},"y":${vals[1]},"w":${vals[2]},"h":${vals[3]}}`;
    return`[${vals.join(',')}]`;
  } else if(el.type==='color'){
    let hx=el.value;
    return`[${parseInt(hx.substr(1,2),16)/255},${parseInt(hx.substr(3,2),16)/255},${parseInt(hx.substr(5,2),16)/255},1]`;
  } else if(el.type==='checkbox'){
    return el.checked?'true':'false';
  }
  return el.value;
}

function process_save(el){
  let val=get_val_from_row(el);
  if(el.classList.contains('vec-input'))el=el.parentElement.parentElement.querySelectorAll('input.vec-input')[0];
  dispatch_update(el,val);
  let orig=el.style.backgroundColor;
  el.style.backgroundColor='#262626';
  setTimeout(()=>{el.style.backgroundColor=orig;},350);
}

function dispatch_update(el,val){
  let tt=el.getAttribute('data-tt');
  let addr=el.getAttribute('data-addr');
  let prop=el.getAttribute('data-pid');
  let vt=el.getAttribute('data-vt');
  let is_prop=el.getAttribute('data-ip')==='true';
  let is_static=el.getAttribute('data-static')==='true';
  let cns=el.getAttribute('data-cns')||'';
  let ccls=el.getAttribute('data-ccls')||'';
  let casm=el.getAttribute('data-casm')||'';
  if(tt==='transform')update_go(addr,'transform',prop,val);
  else if(tt==='instance')update_inst_field(addr,prop,vt,is_prop,val,is_static);
  else if(tt==='script')update_script_field(addr,prop,vt,is_prop,val,cns,ccls,casm);
}

async function update_script_field(addr,name,ftype,is_prop,val,cns,ccls,casm){
  await fetch('/api/scene/update',{method:'POST',headers:{'Content-Type':'application/json'},body:JSON.stringify({addr,type:'script',prop:name,prop2:name,ftype,isProp:is_prop,val:String(val),compNs:cns,compCls:ccls,compAsm:casm||''})}).catch(()=>({}));
}

async function update_inst_field(addr,name,ftype,is_prop,val,is_static){
  let asm=document.getElementById('ctrl-asm').value;
  let ns=document.getElementById('ctrl-ns').value;
  let cls=document.getElementById('ctrl-cls').value;
  await fetch('/api/instance/update',{method:'POST',headers:{'Content-Type':'application/json'},body:JSON.stringify({addr,asm,ns,cls,name,ftype,isProp:is_prop,val:String(val),isStatic:is_static?'true':'false'})});
}

function prompt_loop(btn){
  let el=btn.parentElement.querySelector('.vec-input,.sing-input');
  if(!el)return;
  let val=get_val_from_row(el);
  if(el.classList.contains('vec-input'))el=el.parentElement.parentElement.querySelectorAll('input.vec-input')[0];
  let ms=prompt('Loop interval in milliseconds:','100');
  if(!ms)return;
  let interval=parseInt(ms)||100;
  let addr=el.getAttribute('data-addr');
  let tt=el.getAttribute('data-tt');
  let name=el.getAttribute('data-pid');
  let ftype=el.getAttribute('data-vt');
  let is_prop=el.getAttribute('data-ip')==='true';
  let cns=el.getAttribute('data-cns')||'';
  let ccls=el.getAttribute('data-ccls')||'';
  let casm=el.getAttribute('data-casm')||'';
  let asm=document.getElementById('ctrl-asm')?.value||'';
  let ns=document.getElementById('ctrl-ns')?.value||cns;
  let cls=document.getElementById('ctrl-cls')?.value||ccls;
  if(tt==='script'){ns=cns;cls=ccls;asm=casm;}
  fetch('/api/loop/add',{method:'POST',headers:{'Content-Type':'application/json'},body:JSON.stringify({addr,asm,ns,cls,name,ftype,isProp:is_prop,val,interval})}).then(r=>r.json()).then(d=>{
    if(d.ok){btn.style.background='var(--grn)';refresh_loops_ui();}
  });
}

async function refresh_loops_ui(){
  let loops=await fetch('/api/loop/list').then(r=>r.json());
  let el=document.getElementById('loops-list');
  if(!loops.length){el.innerHTML='<div style="color:var(--fg-dim);padding:14px;text-align:center;">No active memory loops</div>';return;}
  el.innerHTML=loops.map(lp=>`
    <div class="loop-row">
      <span class="g-icon" style="color:var(--mth);font-size:14px;">sync</span>
      <span class="loop-id">${esc(lp.cls)}.${esc(lp.name)}</span>
      <span style="color:var(--str);font-family:Consolas,monospace;font-size:11px;">${esc(lp.val)}</span>
      <span style="color:var(--fg-dim);font-size:10px;">${lp.interval}ms</span>
      <button class="btn btn-red" style="padding:2px 8px;" onclick="remove_loop('${esc(lp.id)}',this)"><span class="g-icon">close</span> Stop</button>
    </div>`).join('');
}

async function remove_loop(id,btn){
  await fetch('/api/loop/remove',{method:'POST',headers:{'Content-Type':'application/json'},body:JSON.stringify({id})});
  btn?.closest('.loop-row')?.remove();
}

async function remove_all_loops(){
  await fetch('/api/loop/removeall',{method:'POST',headers:{'Content-Type':'application/json'},body:'{}'});
  refresh_loops_ui();
}

async function refresh_catcher(){
  let entries=await fetch('/api/catcher/list').then(r=>r.json());
  let el=document.getElementById('catcher-list');
  if(!entries.length){el.innerHTML='<div style="padding:30px;text-align:center;color:var(--fg-dim);">No hooked methods yet.<br>Go to Controller or Scene, click the <span style="color:var(--kw);">&#9679;</span> target button on any method.</div>';return;}
  el.innerHTML=entries.map(e=>{
    let uid=uid8();
    let path=`${e.ns?e.ns+'.':''}${e.cls}.${e.method}`;
    let params=e.params?.map(p=>`<span style="color:var(--type)">${esc(p.t)}</span> ${esc(p.n)}`).join(', ')||'';
    return`<div class="catcher-entry">
      <div class="catcher-hdr" onclick="load_catcher_calls('${esc(e.id)}','${uid}')">
        <div class="ico" style="color:var(--blu2)"><span class="g-icon">track_changes</span></div>
        <span style="color:var(--mth);font-weight:700;">${esc(path)}</span>
        <span style="color:var(--type);font-size:10px;margin-left:4px;">(${params})</span>
        <span style="color:var(--str);font-size:10px;margin-left:4px;">-&gt; ${esc(e.ret)}</span>
        <span style="background:var(--bg);padding:1px 6px;margin-left:auto;border-radius:var(--rad-sm);font-weight:600;">${e.count}</span>
        <button class="btn btn-red" style="padding:2px 8px;margin-left:6px;" onclick="event.stopPropagation();unhook_catcher('${esc(e.id)}')"><span class="g-icon">delete</span></button>
        <button class="btn" style="padding:2px 8px;" onclick="event.stopPropagation();clear_catcher('${esc(e.id)}')"><span class="g-icon">clear_all</span></button>
      </div>
      <div class="catcher-body" id="${uid}"></div>
    </div>`;
  }).join('');
}

async function load_catcher_calls(id,uid){
  let el=document.getElementById(uid);
  if(el.classList.contains('open')){el.classList.remove('open');return;}
  el.classList.add('open');
  let calls=await fetch(`/api/catcher/calls?id=${encodeURIComponent(id)}`).then(r=>r.json());
  if(!calls.length){el.innerHTML='<div class="call-row" style="color:var(--fg-dim);">No calls recorded yet</div>';return;}
  el.innerHTML=calls.slice().reverse().map(c=>{
    let t=new Date(c.ts*1000).toLocaleTimeString();
    let args=c.args?.map((a,i)=>{
      let v=typeof a.v==='object'?JSON.stringify(a.v):String(a.v);
      return`<span style="color:var(--type)">${esc(a.t)}</span>: <span style="color:var(--num)">${esc(v)}</span>`;
    }).join(', ')||'<span style="color:var(--fg-dim);">no args</span>';
    return`<div class="call-row">
      <div style="display:flex;justify-content:space-between;align-items:center;">
        <span class="call-inst" style="color:var(--type);font-weight:600;">${esc(c.instance)}</span>
        <span class="call-ts" style="color:var(--fg-dim);font-size:10px;">${t}</span>
      </div>
      <div class="call-args">(${args})</div>
    </div>`;
  }).join('');
}

async function unhook_catcher(id){
  await fetch('/api/catcher/unhook',{method:'POST',headers:{'Content-Type':'application/json'},body:JSON.stringify({id})});
  refresh_catcher();
}

async function clear_catcher(id){
  await fetch('/api/catcher/clear',{method:'POST',headers:{'Content-Type':'application/json'},body:JSON.stringify({id})});
  refresh_catcher();
}

async function unhook_all(){
  let entries=await fetch('/api/catcher/list').then(r=>r.json());
  for(let e of entries)await fetch('/api/catcher/unhook',{method:'POST',headers:{'Content-Type':'application/json'},body:JSON.stringify({id:e.id})});
  refresh_catcher();
}

function download_dump(){
  let a=document.createElement('a');
  a.href='/api/dump';a.download='dump.cs';a.click();
}

async function run_lua(){
  let code=document.getElementById('lua-editor').value;
  if(!code.trim())return;
  let out=document.getElementById('lua-output');
  out.textContent+='> Executing script...\n';
  try{
    let d=await fetch('/api/lua/exec',{method:'POST',headers:{'Content-Type':'application/json'},body:JSON.stringify({code})}).then(r=>r.json());
    out.textContent+=d.output+'\n';
  }catch(e){out.textContent+='Error: '+e.message+'\n';}
  out.scrollTop=out.scrollHeight;
}

function clear_console(){document.getElementById('lua-output').textContent='';}

async function stop_lua(){
  let out=document.getElementById('lua-output');
  try{await fetch('/api/lua/stop',{method:'POST'});out.textContent+='> Tasks stopped\n';}catch(e){out.textContent+='Error: '+e.message+'\n';}
  out.scrollTop=out.scrollHeight;
}

async function reset_lua(){
  let out=document.getElementById('lua-output');
  try{await fetch('/api/lua/reset',{method:'POST'});out.textContent+='> Lua state reset\n';}catch(e){out.textContent+='Error: '+e.message+'\n';}
  out.scrollTop=out.scrollHeight;
}

async function poll_lua(){
  try{
    let d=await fetch('/api/lua/poll').then(r=>r.json());
    if(d.output){
      let out=document.getElementById('lua-output');
      out.textContent+=d.output+'\n';
      out.scrollTop=out.scrollHeight;
    }
  }catch(e){}
}

setInterval(poll_lua,1000);

document.addEventListener('keydown',function(e){
  if(e.ctrlKey&&e.key==='Enter'){
    let el=document.getElementById('lua-editor');
    if(document.activeElement===el)run_lua();
  }
});

init();
</script>
</body>
</html>)HTML";
    return html;
}
