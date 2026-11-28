#include "explorer_html.hpp"

namespace {

const char k_html_a[] = R"HTML(<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width,initial-scale=1.0,maximum-scale=1.0,user-scalable=no">
<title>BNM Explorer</title>
<link rel="preconnect" href="https://fonts.googleapis.com">
<link rel="preconnect" href="https://fonts.gstatic.com" crossorigin>
<link href="https://fonts.googleapis.com/css2?family=Inter:wght@400;500;600&family=JetBrains+Mono:wght@400;500&display=swap" rel="stylesheet">
<style>
:root{
  --bg:#111111;--bg2:#1e1e1e;--bg3:#2b2b2b;--bg4:#404040;--card:#1e1e1e;
  --sel:#336647;--sel-hover:#2d4b50;--sel2:#548054;
  --fg:#d4d4d4;--fg-dim:#969696;--fg-mut:#5c5c5c;
  --kw:#b4b4b4;--type:#86b886;--mth:#c4894a;--str:#7fb0b8;--num:#969696;
  --brd:#404040;--brd-light:#2b2b2b;
  --red:#76410e;--red2:#9b5a1e;--grn:#336647;--grn2:#548054;
  --org:#76410e;--org2:#9b5a1e;--blu:#2d4b50;--blu2:#3d6870;--purp:#2d4b50;
  --prop:#86b886;--field:#b4b4b4;--method:#c4894a;
  --rad-sm:2px;--rad-md:2px;--rad-lg:2px;--rad-xl:3px;
  --mono:'JetBrains Mono',ui-monospace,SFMono-Regular,Consolas,'Courier New',monospace;
}
*{box-sizing:border-box;margin:0;padding:0;font-family:'Inter','Segoe UI',system-ui,-apple-system,sans-serif;-webkit-tap-highlight-color:transparent;}
body{background:var(--bg);color:var(--fg);display:flex;flex-direction:column;height:100vh;overflow:hidden;font-size:11px;line-height:1.35;}
.i{width:13px;height:13px;stroke:currentColor;fill:none;stroke-width:2;stroke-linecap:round;stroke-linejoin:round;flex-shrink:0;vertical-align:middle;}
.toolbar{background:var(--bg2);padding:4px 8px;border-bottom:1px solid var(--brd);display:flex;gap:8px;align-items:center;z-index:20;}
.brand{display:flex;align-items:center;gap:6px;font-weight:600;font-size:11px;letter-spacing:.4px;color:var(--fg);text-transform:uppercase;}
.badge{background:var(--bg);border:1px solid var(--brd);padding:1px 5px;border-radius:var(--rad-sm);font-size:10px;color:var(--fg-dim);font-family:var(--mono);}
.workspace{display:flex;flex:1;overflow:hidden;padding:0;gap:0;position:relative;}
.sidebar-overlay{display:none;position:fixed;top:0;left:0;right:0;bottom:0;background:rgba(0,0,0,.6);z-index:40;}
.sidebar{width:300px;background:var(--bg2);border-right:1px solid var(--brd);display:flex;flex-direction:column;overflow:hidden;flex-shrink:0;transition:transform .15s ease;}
.side-hdr{background:var(--bg2);padding:0;border-bottom:1px solid var(--brd);display:flex;align-items:stretch;}
.side-tab{flex:1;text-align:center;padding:6px 4px;cursor:pointer;color:var(--fg-dim);font-weight:500;border-bottom:2px solid transparent;user-select:none;display:flex;gap:5px;align-items:center;justify-content:center;}
.side-tab:hover{color:var(--fg);background:var(--bg3);}
.side-tab.active{color:var(--fg);border-bottom-color:var(--sel2);background:var(--bg);}
.side-pane{display:none;flex:1;min-height:0;flex-direction:column;}
.side-pane.active{display:flex;}
.side-tools{padding:6px;display:flex;flex-direction:column;gap:5px;border-bottom:1px solid var(--brd);background:var(--bg2);}
.side-tools .hint{color:var(--fg-mut);font-size:10px;line-height:1.4;}
.tree-wrap{flex:1;overflow-y:auto;padding:2px 0;background:var(--bg);}
::-webkit-scrollbar{width:8px;height:8px;}
::-webkit-scrollbar-thumb{background:var(--bg4);border-radius:0;}
::-webkit-scrollbar-thumb:hover{background:var(--fg-mut);}
::-webkit-scrollbar-track{background:var(--bg);}
::-webkit-scrollbar-corner{background:var(--bg);}
.tree-node{list-style:none;padding-left:12px;}
.tree-nc{display:flex;align-items:center;padding:2px 6px;cursor:pointer;white-space:nowrap;user-select:none;border-radius:0;height:20px;}
.tree-nc:hover{background:var(--bg3);}
.tree-nc.active{background:var(--sel);color:#fff;}
.tree-nc .nm{overflow:hidden;text-overflow:ellipsis;}
.arr{display:inline-flex;align-items:center;justify-content:center;width:12px;height:12px;margin-right:2px;color:var(--fg-mut);transition:transform .1s;}
.arr .i{width:11px;height:11px;}
.arr.open{transform:rotate(90deg);}
.ico{width:13px;height:13px;margin-right:5px;display:inline-flex;align-items:center;justify-content:center;flex-shrink:0;color:var(--fg-dim);}
.ico .i{width:13px;height:13px;}
.ico.t-class{color:var(--type);}
.ico.t-struct{color:var(--str);}
.ico.t-enum{color:var(--mth);}
.ico.t-iface{color:var(--fg-dim);}
.kids{display:none;}
.kids.open{display:block;}
.cnt{color:var(--fg-mut);margin-left:6px;font-size:10px;font-family:var(--mono);}
.main-area{flex:1;display:flex;flex-direction:column;overflow:hidden;background:var(--bg);min-width:0;}
.tabs{display:flex;background:var(--bg2);border-bottom:1px solid var(--brd);overflow-x:auto;padding:0;gap:0;}
.tab{padding:6px 10px;background:transparent;border:none;border-bottom:2px solid transparent;cursor:pointer;white-space:nowrap;flex-shrink:0;color:var(--fg-dim);font-weight:500;font-size:11px;display:flex;align-items:center;gap:5px;user-select:none;}
.tab:hover{background:var(--bg3);color:var(--fg);}
.tab.active{background:var(--bg);color:var(--fg);border-bottom-color:var(--sel2);}
.panel{flex:1;overflow:auto;background:var(--bg);display:none;}
.panel.active{display:flex;flex-direction:column;}
.scene-mgr{display:flex;flex:1;overflow:hidden;min-height:0;}
.hier{width:310px;border-right:1px solid var(--brd);display:flex;flex-direction:column;background:var(--bg);flex-shrink:0;min-height:0;}
.hier-hdr{background:var(--bg2);padding:6px;border-bottom:1px solid var(--brd);display:flex;flex-direction:column;gap:5px;}
.hier-title-row{display:flex;justify-content:space-between;align-items:center;gap:6px;}
.hier-title{font-weight:600;color:var(--fg);text-transform:uppercase;font-size:10px;letter-spacing:.5px;}
.insp{flex:1;overflow-y:auto;background:var(--bg);min-width:0;}
.insp-hdr{padding:6px 8px;border-bottom:1px solid var(--brd);display:flex;align-items:center;gap:6px;background:var(--bg2);position:sticky;top:0;z-index:10;}
.insp-hdr .inp{flex:1;font-size:11px;font-weight:600;background:transparent;border:1px solid transparent;}
.insp-hdr .inp:focus{border-color:var(--sel2);background:var(--bg);}
.comp{margin:0;border:none;border-bottom:1px solid var(--brd);background:var(--bg);}
.comp-hdr{background:var(--bg2);padding:4px 8px;display:flex;align-items:center;cursor:pointer;user-select:none;gap:6px;font-weight:600;}
.comp-hdr:hover{background:var(--bg3);}
.comp-body{display:none;padding:0;}
.comp-body.open{display:block;}
.row{display:flex;align-items:center;border-bottom:1px solid var(--brd-light);min-height:24px;}
.row:last-child{border-bottom:none;}
.row:hover{background:var(--bg2);}
.lbl{width:38%;overflow:hidden;text-overflow:ellipsis;white-space:nowrap;padding:3px 8px;color:var(--prop);border-right:1px solid var(--brd-light);flex-shrink:0;font-size:11px;font-family:var(--mono);}
.val{flex:1;display:flex;gap:4px;padding:3px 8px;align-items:center;flex-wrap:wrap;min-width:0;}
.vl{color:var(--fg-mut);margin-right:2px;font-size:10px;}
.code-view{font-family:var(--mono);font-size:11px;line-height:1.55;padding:8px 12px;white-space:pre;background:var(--bg);}
.c-kw{color:var(--kw);font-family:inherit;}
.c-ty{color:var(--type);font-family:inherit;}
.c-mt{color:var(--mth);font-family:inherit;}
.c-st{color:var(--str);font-family:inherit;}
.c-cm{color:var(--fg-mut);font-family:inherit;}
.c-nm{color:var(--num);font-family:inherit;}
.c-fn{color:var(--mth);font-family:inherit;}
.c-gl{color:var(--str);font-family:inherit;}
.c-op{color:var(--fg-dim);font-family:inherit;}
.table{width:100%;border-collapse:collapse;border:1px solid var(--brd);}
.table th,.table td{padding:3px 8px;border:1px solid var(--brd);text-align:left;}
.table th{background:var(--bg2);font-weight:600;color:var(--fg);}
.btn{background:var(--bg3);color:var(--fg);border:1px solid var(--brd);padding:0 8px;height:22px;border-radius:var(--rad-sm);cursor:pointer;display:inline-flex;align-items:center;justify-content:center;gap:5px;outline:none;flex-shrink:0;font-size:11px;font-weight:500;white-space:nowrap;}
.btn:hover{background:var(--bg4);border-color:var(--fg-mut);}
.btn:active{background:var(--bg2);}
.btn-red{background:var(--red);border-color:var(--red2);color:#f0d6b8;}
.btn-red:hover{background:var(--red2);}
.btn-grn{background:var(--grn);border-color:var(--grn2);color:#e3f0e6;}
.btn-grn:hover{background:var(--grn2);}
.btn-org{background:var(--bg3);border-color:var(--org2);color:var(--mth);}
.btn-org:hover{background:var(--red);color:#f0d6b8;}
.btn-blu{background:var(--blu);border-color:var(--blu2);color:#dcecee;}
.btn-blu:hover{background:var(--blu2);}
.btn-live{background:var(--bg3);}
.btn-live.active{background:var(--grn);border-color:var(--grn2);}
.btn-ic{padding:0;width:22px;}
.btn-sm{height:18px;padding:0 6px;font-size:10px;}
.inp{background:var(--bg);border:1px solid var(--brd);color:var(--fg);padding:0 6px;height:22px;border-radius:var(--rad-sm);width:100%;outline:none;font-size:11px;}
.inp:focus{border-color:var(--sel2);}
.inp:disabled{opacity:.45;cursor:not-allowed;}
.inp::placeholder{color:var(--fg-mut);}
select.inp{padding:0 4px;}
.srch{background:var(--bg);border:1px solid var(--brd);color:var(--fg);padding:0 8px;height:22px;border-radius:var(--rad-sm);width:260px;outline:none;font-size:11px;}
.srch:focus{border-color:var(--sel2);}
.ctrl-res{font-family:var(--mono);background:var(--bg);padding:2px 6px;border:1px solid var(--brd);border-radius:var(--rad-sm);flex:1;margin-left:4px;min-height:22px;display:flex;align-items:center;font-size:11px;}
.seg{display:inline-flex;border:1px solid var(--brd);border-radius:var(--rad-sm);overflow:hidden;flex-shrink:0;}
.seg button{background:var(--bg);color:var(--fg-dim);border:none;border-right:1px solid var(--brd);padding:0 8px;height:20px;cursor:pointer;font-size:10px;font-weight:500;}
.seg button:last-child{border-right:none;}
.seg button:hover{color:var(--fg);background:var(--bg3);}
.seg button.on{background:var(--sel);color:#fff;}
.chip{display:inline-flex;align-items:center;gap:4px;height:18px;padding:0 6px;border:1px solid var(--brd);border-radius:var(--rad-sm);background:var(--bg);color:var(--fg-dim);cursor:pointer;font-size:10px;user-select:none;}
.chip:hover{color:var(--fg);}
.chip .sq{width:8px;height:8px;border:1px solid var(--fg-mut);background:transparent;}
.chip.on{color:var(--fg);}
.chip.on .sq{background:currentColor;border-color:currentColor;}
.chip.k-class.on{color:var(--type);}
.chip.k-method.on{color:var(--method);}
.chip.k-field.on{color:var(--field);}
.chip.k-property.on{color:var(--prop);}
.chips{display:flex;gap:4px;flex-wrap:wrap;}
.loop-row{display:flex;align-items:center;gap:6px;padding:3px 8px;border-bottom:1px solid var(--brd-light);}
.loop-row:hover{background:var(--bg2);}
.loop-id{font-family:var(--mono);font-size:10px;flex:1;overflow:hidden;text-overflow:ellipsis;white-space:nowrap;color:var(--type);}
.script-hdr{background:var(--bg2);}
.builtin-hdr{background:var(--bg2);}
.modal-overlay{position:fixed;top:0;left:0;right:0;bottom:0;background:rgba(0,0,0,.7);display:none;z-index:100;align-items:center;justify-content:center;padding:12px;}
.modal-overlay.open{display:flex;}
.modal-box{background:var(--bg2);border:1px solid var(--brd);border-radius:var(--rad-xl);width:460px;max-width:96vw;max-height:82vh;display:flex;flex-direction:column;overflow:hidden;box-shadow:0 8px 24px rgba(0,0,0,.6);}
.modal-hdr{padding:6px 10px;background:var(--bg3);border-bottom:1px solid var(--brd);display:flex;justify-content:space-between;align-items:center;font-weight:600;}
.modal-body{flex:1;overflow-y:auto;padding:4px;background:var(--bg);}
.add-comp-cat-hdr{padding:4px 8px;background:var(--bg2);border-bottom:1px solid var(--brd);cursor:pointer;font-weight:600;margin-bottom:1px;}
.add-comp-cat-hdr:hover{background:var(--bg3);}
.add-comp-items{display:none;margin-bottom:2px;}
.add-comp-items.open{display:block;}
.add-comp-item{padding:3px 14px;cursor:pointer;border-bottom:1px solid var(--brd-light);}
.add-comp-item:hover{background:var(--sel);color:#fff;}
.coll-row{padding:4px 8px;border-bottom:1px solid var(--brd-light);display:flex;align-items:center;gap:8px;}
.coll-row.clk{cursor:pointer;}
.coll-row.clk:hover{background:var(--sel);}
.coll-idx{min-width:34px;color:var(--fg-mut);font-family:var(--mono);font-size:10px;word-break:break-all;}
.coll-val{flex:1;overflow:hidden;text-overflow:ellipsis;white-space:nowrap;color:var(--fg);}
.coll-ty{color:var(--type);font-size:10px;white-space:nowrap;}
.inst-item{padding:4px 8px;cursor:pointer;border-bottom:1px solid var(--brd-light);display:flex;align-items:center;justify-content:space-between;gap:8px;}
.inst-item:hover{background:var(--sel);color:#fff;}
.btn-freeze{background:var(--bg3);border-color:var(--red2);color:var(--mth);}
.btn-freeze.active{background:var(--red);border-color:var(--red2);color:#f0d6b8;}
.btn-dump{background:var(--blu);border-color:var(--blu2);color:#dcecee;}
.btn-dump:hover{background:var(--blu2);}
.hier-btns{display:flex;gap:3px;flex-wrap:wrap;}
.dump-pre{margin:0;white-space:pre;font-family:var(--mono);font-size:10px;color:var(--fg);overflow:auto;flex:1;padding:6px;background:var(--bg);}
.calls-row{display:flex;gap:6px;align-items:baseline;padding:2px 8px;border-bottom:1px solid var(--brd-light);font-family:var(--mono);font-size:10px;white-space:nowrap;}
.calls-row:hover{background:var(--bg2);}
.calls-row .cr-t{color:var(--fg-mut);min-width:58px;}
.calls-row .cr-c{color:var(--type);}
.calls-row .cr-f{color:var(--mth);font-weight:600;}
.calls-row .cr-a{color:var(--str);white-space:normal;word-break:break-all;}
.calls-row .cr-cb{color:var(--fg-dim);cursor:pointer;margin-left:auto;padding-left:12px;}
.calls-row .cr-cb:hover{color:var(--fg);text-decoration:underline;}
.calls-m{display:flex;align-items:center;gap:6px;padding:3px 8px;border-bottom:1px solid var(--brd-light);font-size:11px;}
.calls-m.ign{opacity:.45;}
.callers{padding:4px 8px;background:var(--bg2);border-bottom:1px solid var(--brd);font-family:var(--mono);font-size:10px;display:flex;gap:10px;flex-wrap:wrap;}
.callers span{cursor:pointer;color:var(--fg-dim);}
.callers span:hover{color:var(--fg);}
.callers b{color:var(--mth);font-weight:600;margin-left:3px;}
.pulse-dot{width:7px;height:7px;border-radius:50%;background:var(--sel2);display:inline-block;margin-right:3px;}
.pulse-dot.off{background:var(--fg-mut);}
.mono-tag{font-size:9px;background:var(--bg);border:1px solid var(--brd);color:var(--fg-dim);padding:0 4px;border-radius:var(--rad-sm);margin-left:4px;font-family:var(--mono);}
.mobile-only{display:none;}
.vlist{flex:1;overflow:auto;position:relative;background:var(--bg);}
.vlist-sp{position:relative;width:100%;}
.vrow{position:absolute;left:0;right:0;height:20px;display:flex;align-items:center;white-space:nowrap;cursor:pointer;user-select:none;padding-right:6px;}
.vrow:hover{background:var(--bg3);}
.vrow.active{background:var(--sel);color:#fff;}
.vrow.off{opacity:.45;}
.vrow .nm{overflow:hidden;text-overflow:ellipsis;}
.vrow .guide{position:absolute;top:0;bottom:0;width:1px;background:var(--brd-light);}
.vrow .cn{color:var(--fg-mut);font-size:10px;margin-left:5px;font-family:var(--mono);}
.sr-row{display:flex;align-items:center;gap:6px;padding:0 8px;height:20px;cursor:pointer;white-space:nowrap;overflow:hidden;}
.sr-row:hover{background:var(--bg3);}
.sr-k{width:11px;height:11px;flex-shrink:0;display:inline-block;border:1px solid currentColor;}
.sr-k.class{color:var(--type);background:var(--type);}
.sr-k.method{color:var(--method);}
.sr-k.field{color:var(--field);border-radius:50%;}
.sr-k.property{color:var(--prop);transform:rotate(45deg) scale(.8);}
.sr-n{color:var(--fg);overflow:hidden;text-overflow:ellipsis;}
.sr-t{color:var(--fg-mut);overflow:hidden;text-overflow:ellipsis;font-family:var(--mono);font-size:10px;}
.sr-c{color:var(--fg-dim);margin-left:auto;padding-left:8px;font-size:10px;overflow:hidden;text-overflow:ellipsis;}
.mem-bar{display:flex;gap:6px;align-items:center;flex-wrap:wrap;padding:4px 8px;background:var(--bg2);border-bottom:1px solid var(--brd);}
.mem-bar .grow{flex:1;min-width:120px;}
.mem-head{padding:6px 8px;background:var(--bg2);border-bottom:1px solid var(--brd);display:flex;flex-direction:column;gap:3px;}
.mem-title{font-size:12px;font-weight:600;color:var(--type);font-family:var(--mono);}
.mem-sub{color:var(--fg-dim);font-size:10px;font-family:var(--mono);}
.mem-name{display:flex;align-items:center;gap:6px;flex-wrap:wrap;}
.mem-name .lab{color:var(--fg-dim);}
.mrow{display:flex;align-items:flex-start;border-bottom:1px solid var(--brd-light);min-height:24px;}
.mrow:hover{background:var(--bg2);}
.mrow .mn{width:42%;max-width:380px;min-width:120px;padding:4px 8px;font-family:var(--mono);font-size:11px;word-break:break-all;flex-shrink:0;}
.mrow .mn .d{color:var(--fg-mut);}
.mrow .mn.k-property{color:var(--prop);}
.mrow .mn.k-field{color:var(--field);}
.mrow .mn.k-method{color:var(--method);}
.mrow .mv{flex:1;display:flex;flex-direction:column;gap:3px;padding:3px 8px;min-width:0;}
.mrow .mline{display:flex;gap:4px;align-items:center;flex-wrap:wrap;}
.mrow .ty{color:var(--str);font-size:10px;font-family:var(--mono);}
.mrow .ev{color:var(--fg-mut);font-size:10px;font-family:var(--mono);}
.mrow .ev.ok{color:var(--fg);}
.mrow input[type=text],.mrow .inp{width:auto;min-width:80px;flex:1;max-width:340px;}
.mrow .sc{font-size:9px;color:var(--fg-mut);margin-left:4px;}
.hl-wrap{position:relative;flex:1;min-width:0;min-height:0;background:var(--bg);overflow:hidden;}
.hl-pre{position:absolute;top:0;left:0;right:0;bottom:0;margin:0;padding:10px;font-family:var(--mono);font-size:12px;line-height:18px;white-space:pre;overflow:hidden;color:var(--fg);pointer-events:none;tab-size:2;}
.ac-pop{position:absolute;z-index:60;background:var(--bg2);border:1px solid var(--brd);min-width:180px;max-width:420px;max-height:196px;overflow-y:auto;font-family:var(--mono);font-size:11px;box-shadow:0 4px 14px rgba(0,0,0,.6);display:none;}
.ac-pop.open{display:block;}
.ac-item{padding:2px 8px;display:flex;gap:8px;cursor:pointer;white-space:nowrap;align-items:baseline;}
.ac-item.sel{background:var(--sel);color:#fff;}
.ac-item .k{color:var(--fg-mut);font-size:9px;min-width:28px;}
.ac-item .d{color:var(--fg-mut);margin-left:auto;padding-left:10px;overflow:hidden;text-overflow:ellipsis;}
.ac-foot{padding:2px 8px;color:var(--fg-mut);font-size:9px;border-top:1px solid var(--brd);background:var(--bg);position:sticky;bottom:0;}
@media (max-width:860px){
  .mobile-only{display:inline-flex;}
  .sidebar{position:fixed;top:30px;left:0;bottom:0;width:84vw;max-width:340px;z-index:50;transform:translateX(-100%);box-shadow:4px 0 18px rgba(0,0,0,.7);}
  .sidebar.open{transform:translateX(0);}
  .sidebar-overlay.open{display:block;}
  .scene-mgr{flex-direction:column;}
  .hier{width:100%;border-right:none;border-bottom:1px solid var(--brd);max-height:42vh;}
  .insp{flex:1;}
  .srch{width:140px;}
  .btn{height:26px;padding:0 10px;}
  .row{flex-direction:column;align-items:stretch;}
  .lbl{width:100%;border-right:none;border-bottom:1px solid var(--brd-light);}
  .val{width:100%;}
  .mrow{flex-direction:column;}
  .mrow .mn{width:100%;max-width:none;padding-bottom:0;}
}
.ws-bar{background:var(--bg2);padding:4px 8px;border-bottom:1px solid var(--brd);display:flex;gap:5px;align-items:center;flex-wrap:wrap;}
.ws-title{font-weight:600;margin-right:2px;text-transform:uppercase;font-size:10px;letter-spacing:.5px;}
.ws-body{display:flex;flex:1;min-height:0;overflow:hidden;position:relative;}
.ws-side{width:230px;flex-shrink:0;border-right:1px solid var(--brd);display:flex;flex-direction:column;background:var(--bg2);min-height:0;}
.ws-stabs{display:flex;border-bottom:1px solid var(--brd);}
.ws-stab{flex:1;text-align:center;padding:5px 4px;cursor:pointer;color:var(--fg-dim);font-weight:500;border-bottom:2px solid transparent;user-select:none;}
.ws-stab:hover{color:var(--fg);}
.ws-stab.active{color:var(--fg);border-bottom-color:var(--sel2);background:var(--bg);}
.ws-pane{flex:1;display:flex;flex-direction:column;min-height:0;}
.ws-actions{display:flex;gap:2px;padding:3px 6px;border-bottom:1px solid var(--brd);align-items:center;}
.ws-actions .grow{flex:1;font-size:10px;color:var(--fg-mut);letter-spacing:.5px;text-transform:uppercase;}
.ws-tree{flex:1;overflow-y:auto;padding:2px 0;background:var(--bg);}
.ws-row{display:flex;align-items:center;gap:5px;padding:2px 6px;cursor:pointer;white-space:nowrap;user-select:none;height:20px;}
.ws-row:hover{background:var(--bg3);}
.ws-row.active{background:var(--sel);color:#fff;}
.ws-row .nm{flex:1;overflow:hidden;text-overflow:ellipsis;}
.ws-row .acts{display:none;gap:1px;}
.ws-row:hover .acts,.ws-row.active .acts{display:flex;}
.ws-ibtn{background:transparent;border:none;color:var(--fg-dim);cursor:pointer;padding:2px;border-radius:var(--rad-sm);display:inline-flex;align-items:center;justify-content:center;}
.ws-ibtn:hover{background:var(--bg4);color:var(--fg);}
.ws-main{flex:1;display:flex;flex-direction:column;min-width:0;min-height:0;}
.ws-tabs{display:flex;align-items:stretch;background:var(--bg2);border-bottom:1px solid var(--brd);min-height:26px;overflow-x:auto;}
.ws-tab{display:flex;align-items:center;gap:6px;padding:0 10px;border-right:1px solid var(--brd);cursor:pointer;color:var(--fg-dim);white-space:nowrap;font-size:11px;border-bottom:2px solid transparent;}
.ws-tab:hover{background:var(--bg3);color:var(--fg);}
.ws-tab.active{background:var(--bg);color:var(--fg);border-bottom-color:var(--sel2);}
.ws-tab .dirty{width:6px;height:6px;border-radius:50%;background:var(--mth);flex-shrink:0;}
.ws-edit{flex:1;display:flex;min-height:0;position:relative;background:var(--bg);}
.ws-gutter{margin:0;padding:10px 8px 10px 10px;min-width:42px;text-align:right;color:var(--fg-mut);background:var(--bg);border-right:1px solid var(--brd);font-family:var(--mono);font-size:12px;line-height:18px;overflow:hidden;user-select:none;}
#lua-editor{position:absolute;top:0;left:0;right:0;bottom:0;width:100%;height:100%;background:transparent;color:transparent;caret-color:var(--fg);border:none;padding:10px;font-family:var(--mono);font-size:12px;line-height:18px;resize:none;outline:none;tab-size:2;white-space:pre;overflow:auto;}
#lua-editor::selection{background:rgba(84,128,84,.45);color:transparent;}
.ws-empty{position:absolute;top:0;left:0;right:0;bottom:0;display:flex;align-items:center;justify-content:center;color:var(--fg-mut);background:var(--bg);z-index:5;}
.ws-out-hdr{display:flex;align-items:center;gap:6px;padding:2px 8px;background:var(--bg2);border-top:1px solid var(--brd);border-bottom:1px solid var(--brd);font-size:10px;color:var(--fg-dim);letter-spacing:.5px;text-transform:uppercase;}
#lua-output{height:30%;min-height:80px;background:var(--bg);color:var(--type);padding:6px 10px;margin:0;overflow:auto;font-family:var(--mono);font-size:11px;white-space:pre-wrap;}
.ws-zip-row{padding:6px 8px;border-bottom:1px solid var(--brd-light);display:flex;flex-direction:column;gap:4px;}
.ws-zip-name{font-weight:500;word-break:break-all;}
.ws-zip-meta{font-size:10px;color:var(--fg-mut);}
.ws-zip-btns{display:flex;gap:3px;}
.ws-hint{padding:6px 8px;font-size:10px;color:var(--fg-mut);line-height:1.5;word-break:break-all;}
.ws-dialog-msg{padding:10px 12px;line-height:1.5;color:var(--fg);}
.ws-dialog-foot{display:flex;justify-content:flex-end;gap:5px;padding:8px 10px;border-top:1px solid var(--brd);}
@media (max-width:860px){
  .ws-side{position:absolute;top:0;bottom:0;left:0;width:80%;max-width:290px;z-index:30;transform:translateX(-102%);transition:transform .15s ease;}
  .ws-side.show{transform:none;box-shadow:0 0 24px rgba(0,0,0,.7);}
}
@media (min-width:861px){.ws-toggle{display:none;}}
</style>
</head>
<body>
<svg width=0 height=0 style="position:absolute" aria-hidden="true"><defs>
<symbol id="i-activity" viewBox="0 0 24 24"><path d="M22 12h-2.48a2 2 0 0 0-1.93 1.46l-2.35 8.36a.25.25 0 0 1-.48 0L9.24 2.18a.25.25 0 0 0-.48 0l-2.35 8.36A2 2 0 0 1 4.49 12H2" /></symbol>
<symbol id="i-ban" viewBox="0 0 24 24"><circle cx="12" cy="12" r="10" /> <path d="M4.929 4.929 19.07 19.071" /></symbol>
<symbol id="i-box" viewBox="0 0 24 24"><path d="M21 8a2 2 0 0 0-1-1.73l-7-4a2 2 0 0 0-2 0l-7 4A2 2 0 0 0 3 8v8a2 2 0 0 0 1 1.73l7 4a2 2 0 0 0 2 0l7-4A2 2 0 0 0 21 16Z" /> <path d="m3.3 7 8.7 5 8.7-5" /> <path d="M12 22V12" /></symbol>
<symbol id="i-check" viewBox="0 0 24 24"><path d="M20 6 9 17l-5-5" /></symbol>
<symbol id="i-chevron-right" viewBox="0 0 24 24"><path d="m9 18 6-6-6-6" /></symbol>
<symbol id="i-circle-dot" viewBox="0 0 24 24"><circle cx="12" cy="12" r="1" /> <circle cx="12" cy="12" r="10" /></symbol>
<symbol id="i-code-xml" viewBox="0 0 24 24"><path d="m18 16 4-4-4-4" /> <path d="m6 8-4 4 4 4" /> <path d="m14.5 4-5 16" /></symbol>
<symbol id="i-copy" viewBox="0 0 24 24"><rect width="14" height="14" x="8" y="8" rx="2" ry="2" /> <path d="M4 16c-1.1 0-2-.9-2-2V4c0-1.1.9-2 2-2h10c1.1 0 2 .9 2 2" /></symbol>
<symbol id="i-crosshair" viewBox="0 0 24 24"><circle cx="12" cy="12" r="10" /> <line x1="22" x2="18" y1="12" y2="12" /> <line x1="6" x2="2" y1="12" y2="12" /> <line x1="12" x2="12" y1="6" y2="2" /> <line x1="12" x2="12" y1="22" y2="18" /></symbol>
<symbol id="i-database" viewBox="0 0 24 24"><ellipse cx="12" cy="5" rx="9" ry="3" /> <path d="M3 5V19A9 3 0 0 0 21 19V5" /> <path d="M3 12A9 3 0 0 0 21 12" /></symbol>
<symbol id="i-download" viewBox="0 0 24 24"><path d="M12 15V3" /> <path d="M21 15v4a2 2 0 0 1-2 2H5a2 2 0 0 1-2-2v-4" /> <path d="m7 10 5 5 5-5" /></symbol>
<symbol id="i-external-link" viewBox="0 0 24 24"><path d="M15 3h6v6" /> <path d="M10 14 21 3" /> <path d="M18 13v6a2 2 0 0 1-2 2H5a2 2 0 0 1-2-2V8a2 2 0 0 1 2-2h6" /></symbol>
<symbol id="i-eye" viewBox="0 0 24 24"><path d="M2.062 12.348a1 1 0 0 1 0-.696 10.75 10.75 0 0 1 19.876 0 1 1 0 0 1 0 .696 10.75 10.75 0 0 1-19.876 0" /> <circle cx="12" cy="12" r="3" /></symbol>
<symbol id="i-eye-off" viewBox="0 0 24 24"><path d="M10.733 5.076a10.744 10.744 0 0 1 11.205 6.575 1 1 0 0 1 0 .696 10.747 10.747 0 0 1-1.444 2.49" /> <path d="M14.084 14.158a3 3 0 0 1-4.242-4.242" /> <path d="M17.479 17.499a10.75 10.75 0 0 1-15.417-5.151 1 1 0 0 1 0-.696 10.75 10.75 0 0 1 4.446-5.143" /> <path d="m2 2 20 20" /></symbol>
<symbol id="i-file-plus" viewBox="0 0 24 24"><path d="M6 22a2 2 0 0 1-2-2V4a2 2 0 0 1 2-2h8a2.4 2.4 0 0 1 1.704.706l3.588 3.588A2.4 2.4 0 0 1 20 8v12a2 2 0 0 1-2 2z" /> <path d="M14 2v5a1 1 0 0 0 1 1h5" /> <path d="M9 15h6" /> <path d="M12 18v-6" /></symbol>
<symbol id="i-file-text" viewBox="0 0 24 24"><path d="M6 22a2 2 0 0 1-2-2V4a2 2 0 0 1 2-2h8a2.4 2.4 0 0 1 1.704.706l3.588 3.588A2.4 2.4 0 0 1 20 8v12a2 2 0 0 1-2 2z" /> <path d="M14 2v5a1 1 0 0 0 1 1h5" /> <path d="M10 9H8" /> <path d="M16 13H8" /> <path d="M16 17H8" /></symbol>
<symbol id="i-file-up" viewBox="0 0 24 24"><path d="M6 22a2 2 0 0 1-2-2V4a2 2 0 0 1 2-2h8a2.4 2.4 0 0 1 1.704.706l3.588 3.588A2.4 2.4 0 0 1 20 8v12a2 2 0 0 1-2 2z" /> <path d="M14 2v5a1 1 0 0 0 1 1h5" /> <path d="M12 12v6" /> <path d="m15 15-3-3-3 3" /></symbol>
<symbol id="i-folder" viewBox="0 0 24 24"><path d="M20 20a2 2 0 0 0 2-2V8a2 2 0 0 0-2-2h-7.9a2 2 0 0 1-1.69-.9L9.6 3.9A2 2 0 0 0 7.93 3H4a2 2 0 0 0-2 2v13a2 2 0 0 0 2 2Z" /></symbol>
<symbol id="i-folder-open" viewBox="0 0 24 24"><path d="m6 14 1.5-2.9A2 2 0 0 1 9.24 10H20a2 2 0 0 1 1.94 2.5l-1.54 6a2 2 0 0 1-1.95 1.5H4a2 2 0 0 1-2-2V5a2 2 0 0 1 2-2h3.9a2 2 0 0 1 1.69.9l.81 1.2a2 2 0 0 0 1.67.9H18a2 2 0 0 1 2 2v2" /></symbol>
<symbol id="i-folder-plus" viewBox="0 0 24 24"><path d="M12 10v6" /> <path d="M9 13h6" /> <path d="M20 20a2 2 0 0 0 2-2V8a2 2 0 0 0-2-2h-7.9a2 2 0 0 1-1.69-.9L9.6 3.9A2 2 0 0 0 7.93 3H4a2 2 0 0 0-2 2v13a2 2 0 0 0 2 2Z" /></symbol>
<symbol id="i-gamepad-2" viewBox="0 0 24 24"><line x1="6" x2="10" y1="11" y2="11" /> <line x1="8" x2="8" y1="9" y2="13" /> <line x1="15" x2="15.01" y1="12" y2="12" /> <line x1="18" x2="18.01" y1="10" y2="10" /> <path d="M17.32 5H6.68a4 4 0 0 0-3.978 3.59c-.006.052-.01.101-.017.152C2.604 9.416 2 14.456 2 16a3 3 0 0 0 3 3c1 0 1.5-.5 2-1l1.414-1.414A2 2 0 0 1 9.828 16h4.344a2 2 0 0 1 1.414.586L17 18c.5.5 1 1 2 1a3 3 0 0 0 3-3c0-1.545-.604-6.584-.685-7.258-.007-.05-.011-.1-.017-.151A4 4 0 0 0 17.32 5z" /></symbol>
<symbol id="i-layers" viewBox="0 0 24 24"><path d="M12.83 2.18a2 2 0 0 0-1.66 0L2.6 6.08a1 1 0 0 0 0 1.83l8.58 3.91a2 2 0 0 0 1.66 0l8.58-3.9a1 1 0 0 0 0-1.83z" /> <path d="M2 12a1 1 0 0 0 .58.91l8.6 3.91a2 2 0 0 0 1.65 0l8.58-3.9A1 1 0 0 0 22 12" /> <path d="M2 17a1 1 0 0 0 .58.91l8.6 3.91a2 2 0 0 0 1.65 0l8.58-3.9A1 1 0 0 0 22 17" /></symbol>
<symbol id="i-list" viewBox="0 0 24 24"><path d="M3 5h.01" /> <path d="M3 12h.01" /> <path d="M3 19h.01" /> <path d="M8 5h13" /> <path d="M8 12h13" /> <path d="M8 19h13" /></symbol>
<symbol id="i-list-x" viewBox="0 0 24 24"><path d="M16 5H3" /> <path d="M11 12H3" /> <path d="M16 19H3" /> <path d="m15.5 9.5 5 5" /> <path d="m20.5 9.5-5 5" /></symbol>
<symbol id="i-menu" viewBox="0 0 24 24"><path d="M4 5h16" /> <path d="M4 12h16" /> <path d="M4 19h16" /></symbol>
<symbol id="i-mouse-pointer-click" viewBox="0 0 24 24"><path d="M14 4.1 12 6" /> <path d="m5.1 8-2.9-.8" /> <path d="m6 12-1.9 2" /> <path d="M7.2 2.2 8 5.1" /> <path d="M9.037 9.69a.498.498 0 0 1 .653-.653l11 4.5a.5.5 0 0 1-.074.949l-4.349 1.041a1 1 0 0 0-.74.739l-1.04 4.35a.5.5 0 0 1-.95.074z" /></symbol>
<symbol id="i-network" viewBox="0 0 24 24"><rect x="16" y="16" width="6" height="6" rx="1" /> <rect x="2" y="16" width="6" height="6" rx="1" /> <rect x="9" y="2" width="6" height="6" rx="1" /> <path d="M5 16v-3a1 1 0 0 1 1-1h12a1 1 0 0 1 1 1v3" /> <path d="M12 12V8" /></symbol>
<symbol id="i-package" viewBox="0 0 24 24"><path d="M11 21.73a2 2 0 0 0 2 0l7-4A2 2 0 0 0 21 16V8a2 2 0 0 0-1-1.73l-7-4a2 2 0 0 0-2 0l-7 4A2 2 0 0 0 3 8v8a2 2 0 0 0 1 1.73z" /> <path d="M12 22V12" /> <polyline points="3.29 7 12 12 20.71 7" /> <path d="m7.5 4.27 9 5.15" /></symbol>
<symbol id="i-image" viewBox="0 0 24 24"><rect width="18" height="18" x="3" y="3" rx="2" ry="2" /><circle cx="9" cy="9" r="2" /><path d="m21 15-3.086-3.086a2 2 0 0 0-2.828 0L6 21" /></symbol>
<symbol id="i-music" viewBox="0 0 24 24"><path d="M9 18V5l12-2v13" /><circle cx="6" cy="18" r="3" /><circle cx="18" cy="16" r="3" /></symbol>
<symbol id="i-pause" viewBox="0 0 24 24"><rect x="14" y="3" width="5" height="18" rx="1" /> <rect x="5" y="3" width="5" height="18" rx="1" /></symbol>
<symbol id="i-pencil" viewBox="0 0 24 24"><path d="M21.174 6.812a1 1 0 0 0-3.986-3.987L3.842 16.174a2 2 0 0 0-.5.83l-1.321 4.352a.5.5 0 0 0 .623.622l4.353-1.32a2 2 0 0 0 .83-.497z" /> <path d="m15 5 4 4" /></symbol>
<symbol id="i-play" viewBox="0 0 24 24"><path d="M5 5a2 2 0 0 1 3.008-1.728l11.997 6.998a2 2 0 0 1 .003 3.458l-12 7A2 2 0 0 1 5 19z" /></symbol>
<symbol id="i-plus" viewBox="0 0 24 24"><path d="M5 12h14" /> <path d="M12 5v14" /></symbol>
<symbol id="i-refresh-cw" viewBox="0 0 24 24"><path d="M3 12a9 9 0 0 1 9-9 9.75 9.75 0 0 1 6.74 2.74L21 8" /> <path d="M21 3v5h-5" /> <path d="M21 12a9 9 0 0 1-9 9 9.75 9.75 0 0 1-6.74-2.74L3 16" /> <path d="M8 16H3v5" /></symbol>
<symbol id="i-repeat" viewBox="0 0 24 24"><path d="m17 2 4 4-4 4" /> <path d="M3 11v-1a4 4 0 0 1 4-4h14" /> <path d="m7 22-4-4 4-4" /> <path d="M21 13v1a4 4 0 0 1-4 4H3" /></symbol>
<symbol id="i-rotate-ccw" viewBox="0 0 24 24"><path d="M3 12a9 9 0 1 0 9-9 9.75 9.75 0 0 0-6.74 2.74L3 8" /> <path d="M3 3v5h5" /></symbol>
<symbol id="i-save" viewBox="0 0 24 24"><path d="M15.2 3a2 2 0 0 1 1.4.6l3.8 3.8a2 2 0 0 1 .6 1.4V19a2 2 0 0 1-2 2H5a2 2 0 0 1-2-2V5a2 2 0 0 1 2-2z" /> <path d="M17 21v-7a1 1 0 0 0-1-1H8a1 1 0 0 0-1 1v7" /> <path d="M7 3v4a1 1 0 0 0 1 1h7" /></symbol>
<symbol id="i-search" viewBox="0 0 24 24"><path d="m21 21-4.34-4.34" /> <circle cx="11" cy="11" r="8" /></symbol>
<symbol id="i-snowflake" viewBox="0 0 24 24"><path d="m10 20-1.25-2.5L6 18" /> <path d="M10 4 8.75 6.5 6 6" /> <path d="m14 20 1.25-2.5L18 18" /> <path d="m14 4 1.25 2.5L18 6" /> <path d="m17 21-3-6h-4" /> <path d="m17 3-3 6 1.5 3" /> <path d="M2 12h6.5L10 9" /> <path d="m20 10-1.5 2 1.5 2" /> <path d="M22 12h-6.5L14 15" /> <path d="m4 10 1.5 2L4 14" /> <path d="m7 21 3-6-1.5-3" /> <path d="m7 3 3 6h4" /></symbol>
<symbol id="i-square" viewBox="0 0 24 24"><rect width="18" height="18" x="3" y="3" rx="2" /></symbol>
<symbol id="i-terminal" viewBox="0 0 24 24"><path d="M12 19h8" /> <path d="m4 17 6-6-6-6" /></symbol>
<symbol id="i-trash-2" viewBox="0 0 24 24"><path d="M10 11v6" /> <path d="M14 11v6" /> <path d="M19 6v14a2 2 0 0 1-2 2H7a2 2 0 0 1-2-2V6" /> <path d="M3 6h18" /> <path d="M8 6V4a2 2 0 0 1 2-2h4a2 2 0 0 1 2 2v2" /></symbol>
<symbol id="i-x" viewBox="0 0 24 24"><path d="M18 6 6 18" /> <path d="m6 6 12 12" /></symbol>
</defs></svg>
<div class="sidebar-overlay" id="sidebar-overlay" onclick="toggle_sidebar(false)"></div>
<div class="toolbar">
  <button class="btn btn-ic mobile-only" onclick="toggle_sidebar(true)"><svg class=i><use href=#i-menu></use></svg></button>
  <div class="brand"><svg class=i><use href=#i-box></use></svg><span>BNM Explorer</span><span class="badge">)HTML";

const char k_html_b[] = R"HTML( asm</span></div>
  <span style="flex:1;"></span>
  <input type="text" class="srch" id="gSearch" placeholder="Search classes and members, Enter" onkeyup="do_search(event)">
  <button class="btn" onclick="download_dump()" title="Download dump.cs"><svg class=i><use href=#i-download></use></svg> Dump</button>
</div>
<div class="workspace">
  <div class="sidebar" id="app-sidebar">
    <div class="side-hdr">
      <div class="side-tab active" data-t="classes" onclick="side_tab('classes')"><svg class=i><use href=#i-package></use></svg> Classes</div>
      <div class="side-tab" data-t="search" onclick="side_tab('search')"><svg class=i><use href=#i-search></use></svg> Search</div>
      <button class="btn btn-ic mobile-only" style="height:auto;border:none;" onclick="toggle_sidebar(false)"><svg class=i><use href=#i-x></use></svg></button>
    </div>
    <div class="side-pane active" id="side-classes">
      <div class="side-tools">
        <input class="inp" id="tree-q" placeholder="Assembly-CSharp.dll/Photon/Player" oninput="tree_filter_input()">
        <div class="hint">Filter by assembly / namespace / class. Each part is a partial match, one part matches any level.</div>
      </div>
      <div class="tree-wrap" id="tree-root"></div>
    </div>
    <div class="side-pane" id="side-search">
      <div class="side-tools">
        <input class="inp" id="msearch-q" placeholder="Name or Class.member" oninput="ms_input()">
        <input class="inp" id="msearch-path" placeholder="Scope: assembly/namespace/class (optional)" oninput="ms_input()">
        <div class="chips">
          <span class="chip k-class on" id="chip-class" onclick="ms_chip('class')"><span class="sq"></span>Class</span>
          <span class="chip k-method on" id="chip-method" onclick="ms_chip('method')"><span class="sq"></span>Method</span>
          <span class="chip k-field on" id="chip-field" onclick="ms_chip('field')"><span class="sq"></span>Field</span>
          <span class="chip k-property on" id="chip-property" onclick="ms_chip('property')"><span class="sq"></span>Property</span>
        </div>
        <div class="hint" id="msearch-status"></div>
      </div>
      <div class="vlist" id="msearch-list"></div>
    </div>
  </div>
<div class="main-area">
    <div class="tabs">
      <div class="tab active" onclick="switch_tab('scene')" id="tab-scene"><svg class=i><use href=#i-network></use></svg> Scene</div>
      <div class="tab" onclick="switch_tab('code')" id="tab-code"><svg class=i><use href=#i-code-xml></use></svg> Class</div>
      <div class="tab" onclick="switch_tab('inst')" id="tab-inst"><svg class=i><use href=#i-database></use></svg> Instances</div>
      <div class="tab" onclick="switch_tab('ctrl')" id="tab-ctrl"><svg class=i><use href=#i-gamepad-2></use></svg> Inspector</div>
      <div class="tab" onclick="switch_tab('calls')" id="tab-calls"><svg class=i><use href=#i-activity></use></svg> Watcher</div>
      <div class="tab" onclick="switch_tab('loops')" id="tab-loops"><svg class=i><use href=#i-repeat></use></svg> Loops</div>
      <div class="tab" onclick="switch_tab('lua')" id="tab-lua"><svg class=i><use href=#i-terminal></use></svg> Console</div>
      <div class="tab" onclick="switch_tab('ftrack')" id="tab-ftrack"><svg class=i><use href=#i-snowflake></use></svg> Freeze</div>
      <div class="tab" onclick="switch_tab('mgr')" id="tab-mgr"><svg class=i><use href=#i-pause></use></svg> Manager</div>
    </div>

    <div id="boot-banner" style="display:none;background:var(--red);border-bottom:1px solid var(--red2);padding:6px 12px;align-items:center;gap:8px;color:#f0d6b8;font-weight:600;">
      <span style="flex:1;">Game is frozen at startup — hooks can be installed now. Don't touch the device screen while frozen.</span>
      <span id="boot-banner-t" style="font-family:var(--mono);color:#ffffff;"></span>
      <button class="btn btn-grn" onclick="mgr_resume()">Resume game</button>
      <button class="btn" onclick="switch_tab('mgr')">Details</button>
    </div>

    <div class="panel" id="pnl-ftrack" style="padding:0;flex-direction:column;">
      <div style="background:var(--bg3);padding:8px 12px;border-bottom:1px solid var(--brd);display:flex;gap:6px;align-items:center;flex-wrap:wrap;">
        <span style="font-weight:700;">FreezeTracker</span>
        <span id="ftrack-status" style="color:var(--fg-dim);font-size:11px;flex:1;">Enable Freeze in Scene to track blocked attempts</span>
        <input class="inp" id="ftrack-filter" placeholder="Filter..." oninput="ftrack_render()" style="width:160px;">
        <button class="btn" onclick="ftrack_poll()"><svg class=i><use href=#i-refresh-cw></use></svg> Refresh</button>
        <button class="btn btn-red" onclick="ftrack_clear()"><svg class=i><use href=#i-list-x></use></svg> Clear</button>
      </div>
      <div id="ftrack-list" style="flex:1;overflow:auto;background:var(--bg);"></div>
    </div>

    <div class="panel active" id="pnl-scene">
      <div class="scene-mgr">
        <div class="hier">
          <div class="hier-hdr">
            <div class="hier-title-row">
              <span class="hier-title">Hierarchy</span>
              <div class="hier-btns">
                <button class="btn btn-live" id="btn-live-sync" onclick="toggle_live_sync()" title="Toggle live auto sync"><span class="pulse-dot off" id="live-dot"></span> Live</button>
                <button class="btn btn-dump" id="btn-dump" onclick="open_dump()" title="Dump entire scene: GameObjects, components, fields"><svg class=i><use href=#i-network></use></svg> Dump</button>
                <button class="btn btn-freeze" id="btn-freeze" onclick="toggle_freeze()" title="Block Destroy and scene changes"><svg class=i><use href=#i-snowflake></use></svg> <span id="freeze-lbl">Freeze</span></button>
                <button class="btn" onclick="load_scene(true)" title="Reload Scene"><svg class=i><use href=#i-refresh-cw></use></svg></button>
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
                <button class="btn btn-blu" onclick="open_ctrl_instance_picker()" title="Pick live instance"><svg class=i><use href=#i-mouse-pointer-click></use></svg></button>
              </div>
            </div>
            <div style="display:flex;align-items:center;gap:6px;">
              <input type="checkbox" id="ctrl-static" onchange="document.getElementById('ctrl-addr').disabled=this.checked;if(this.checked)document.getElementById('ctrl-addr').value='0x0';">
              <label for="ctrl-static" style="color:var(--fg-dim);font-weight:600;">Static (no instance)</label>
            </div>
            <div><label style="color:var(--fg-dim);display:block;margin-bottom:3px;font-weight:600;">Assembly</label><input class="inp" id="ctrl-asm"></div>
            <div><label style="color:var(--fg-dim);display:block;margin-bottom:3px;font-weight:600;">Namespace</label><input class="inp" id="ctrl-ns"></div>
            <div><label style="color:var(--fg-dim);display:block;margin-bottom:3px;font-weight:600;">Class</label><input class="inp" id="ctrl-cls"></div>
            <button class="btn btn-grn" style="justify-content:center;" onclick="load_controller()"><svg class=i><use href=#i-search></use></svg> Inspect Target</button>
          </div>
        </div>
        <div class="insp" id="ctrl-insp"><div style="padding:30px;text-align:center;color:var(--fg-dim);">Configure target or select from explorer</div></div>
      </div>
    </div>

    <div class="panel" id="pnl-loops" style="padding:10px;flex-direction:column;gap:6px;">
      <div style="display:flex;justify-content:space-between;align-items:center;background:var(--bg3);padding:8px 12px;border-radius:var(--rad-md);border:1px solid var(--brd);">
        <span style="font-weight:700;">Active Memory Loops</span>
        <button class="btn btn-red" onclick="remove_all_loops()"><svg class=i><use href=#i-trash-2></use></svg> Stop All</button>
      </div>
      <div id="loops-list" style="margin-top:6px;"></div>
    </div>

    <div class="panel" id="pnl-calls" style="padding:0;flex-direction:column;">
      <div style="background:var(--bg3);padding:8px 12px;border-bottom:1px solid var(--brd);display:flex;gap:6px;align-items:center;flex-wrap:wrap;">
        <span style="font-weight:600;text-transform:uppercase;font-size:10px;letter-spacing:.5px;">Watcher</span>
        <button class="btn btn-grn" id="calls-scene-btn" onclick="calls_toggle_scene()"><svg class=i><use href=#i-eye></use></svg> Watch scene</button>
        <label style="display:flex;gap:4px;align-items:center;font-size:11px;" title="Hook components of objects that appear later"><input type="checkbox" id="calls-auto" checked> auto-hook new</label>
        <input class="inp" id="calls-ns" placeholder="Namespace" style="width:110px;">
        <input class="inp" id="calls-cls" placeholder="Class" style="width:130px;">
        <button class="btn" onclick="calls_watch()"><svg class=i><use href=#i-plus></use></svg> Watch class</button>
        <button class="btn btn-org" id="calls-pause-btn" onclick="calls_toggle_pause()"><svg class=i><use href=#i-pause></use></svg> Pause</button>
        <button class="btn" onclick="calls_clear()"><svg class=i><use href=#i-list-x></use></svg> Clear</button>
        <button class="btn" onclick="calls_show_methods()"><svg class=i><use href=#i-list></use></svg> Methods</button>
        <button class="btn btn-red" onclick="calls_unwatch(0)"><svg class=i><use href=#i-trash-2></use></svg> Unwatch all</button>
      </div>
      <div style="padding:4px 12px;border-bottom:1px solid var(--brd);display:flex;gap:6px;align-items:center;flex-wrap:wrap;font-size:11px;">
        <span id="calls-status" style="color:var(--fg-dim);flex:1;">Nothing watched yet</span>
        <input class="inp" id="calls-filter" placeholder="Filter log..." oninput="calls_render()" style="width:150px;">
        <span>Rate/s per method</span><input class="inp" id="calls-limit" type="number" value="100" onchange="calls_config()" style="width:70px;">
        <label style="display:flex;gap:4px;align-items:center;"><input type="checkbox" id="calls-follow" checked> follow</label>
      </div>
      <div class="callers" id="calls-callers" style="display:none;"></div>
      <div id="calls-log" style="flex:1;overflow:auto;background:var(--bg);"></div>
    </div>

    <div class="panel" id="pnl-lua" style="padding:0;flex-direction:column;">
      <div class="ws-bar">
        <button class="btn ws-toggle" onclick="ws_toggle_sidebar()"><svg class=i><use href=#i-menu></use></svg></button>
        <span class="ws-title">LuaBNM Console</span>
        <button class="btn btn-grn" onclick="ws_run()"><svg class=i><use href=#i-play></use></svg> Run</button>
        <label class="chip on" id="run-main-chip" onclick="this.classList.toggle('on');ws_run_main=this.classList.contains('on')" title="Run chunks on the game main thread so Unity calls are safe"><span class="sq"></span>Main thread</label>
        <button class="btn btn-red" onclick="stop_lua()"><svg class=i><use href=#i-square></use></svg> Stop</button>
        <button class="btn btn-red" onclick="reset_lua()"><svg class=i><use href=#i-rotate-ccw></use></svg> Reset</button>
        <span style="flex:1;"></span>
        <button class="btn" onclick="ws_import_click()"><svg class=i><use href=#i-file-up></use></svg> Import ZIP</button>
        <button class="btn" onclick="ws_export()"><svg class=i><use href=#i-download></use></svg> Export ZIP</button>
        <button class="btn btn-blu" onclick="ws_save_device()"><svg class=i><use href=#i-save></use></svg> Save On Device</button>
        <input type="file" id="ws-file-input" accept=".zip,application/zip" style="display:none;" onchange="ws_import_selected(this)">
      </div>
      <div class="ws-body">
        <div class="ws-side" id="ws-side">
          <div class="ws-stabs">
            <div class="ws-stab active" id="ws-stab-files" onclick="ws_side_tab('files')">Workspace</div>
            <div class="ws-stab" id="ws-stab-zips" onclick="ws_side_tab('zips')">Device ZIPs</div>
          </div>
          <div class="ws-pane" id="ws-files-pane">
            <div class="ws-actions">
              <span class="grow">Files</span>
              <button class="ws-ibtn" title="New file" onclick="ws_new_file()"><svg class=i><use href=#i-file-plus></use></svg></button>
              <button class="ws-ibtn" title="New folder" onclick="ws_new_folder()"><svg class=i><use href=#i-folder-plus></use></svg></button>
              <button class="ws-ibtn" title="Upload file (any type)" onclick="ws_upload_click()"><svg class=i><use href=#i-file-up></use></svg></button>
              <button class="ws-ibtn" title="Refresh" onclick="ws_refresh()"><svg class=i><use href=#i-refresh-cw></use></svg></button>
              <input type="file" id="ws-upload-input" style="display:none;" onchange="ws_upload_selected(this)">
            </div>
            <div class="ws-tree" id="ws-tree"></div>
          </div>
          <div class="ws-pane" id="ws-zips-pane" style="display:none;">
            <div class="ws-actions">
              <span class="grow">Saved on device</span>
              <button class="ws-ibtn" title="Refresh" onclick="ws_refresh_zips()"><svg class=i><use href=#i-refresh-cw></use></svg></button>
            </div>
            <div class="ws-tree" id="ws-zip-list"></div>
            <div class="ws-hint" id="ws-zip-path"></div>
          </div>
        </div>
        <div class="ws-main">
          <div class="ws-tabs" id="ws-tabs"></div>
          <div class="ws-edit">
            <pre class="ws-gutter" id="ws-gutter">1</pre>
            <div class="hl-wrap"><pre class="hl-pre" id="lua-hl"></pre><textarea id="lua-editor" spellcheck="false" wrap="off" autocomplete="off" autocapitalize="off"></textarea><div class="ac-pop" id="lua-ac"></div></div>
            <div class="ws-empty" id="ws-empty">Open a file from the workspace</div>
          </div>
          <div class="ws-out-hdr">
            <span style="flex:1;">Output</span>
            <button class="ws-ibtn" title="Clear output" onclick="clear_console()"><svg class=i><use href=#i-trash-2></use></svg></button>
          </div>
          <pre id="lua-output"></pre>
        </div>
      </div>
    </div>

    <div class="panel" id="pnl-mgr" style="padding:12px;flex-direction:column;gap:8px;overflow-y:auto;">
      <div style="background:var(--bg3);padding:10px 12px;border:1px solid var(--brd);border-radius:var(--rad-md);display:flex;flex-direction:column;gap:8px;">
        <div style="display:flex;align-items:center;gap:8px;flex-wrap:wrap;">
          <span style="font-weight:700;font-size:12px;">Startup Manager</span>
          <span class="badge" id="mgr-state">?</span>
          <span style="flex:1;"></span>
          <button class="btn btn-grn" id="mgr-resume" onclick="mgr_resume()"><svg class=i><use href=#i-play></use></svg> Resume now</button>
        </div>
        <div style="color:var(--fg-dim);line-height:1.6;">
          Freeze on next launch pauses the game inside the IL2CPP init hook, before any scene or game code runs. The web server keeps working, so you can install Lua hooks or run scripts, then resume. Auto Lua runs a workspace file at the same early point on every launch.
        </div>
      </div>
      <div style="background:var(--bg2);padding:10px 12px;border:1px solid var(--brd);border-radius:var(--rad-md);display:flex;flex-direction:column;gap:8px;">
        <div style="display:flex;align-items:center;gap:8px;flex-wrap:wrap;">
          <input type="checkbox" id="mgr-hold" onchange="mgr_save()">
          <label for="mgr-hold" style="font-weight:600;">Freeze on next launch</label>
          <span style="flex:1;"></span>
          <span style="color:var(--fg-dim);">Timeout (sec, 0 = none)</span>
          <input class="inp" id="mgr-timeout" type="number" value="0" style="width:90px;" onchange="mgr_save()">
        </div>
        <div style="display:flex;align-items:center;gap:8px;flex-wrap:wrap;">
          <input type="checkbox" id="mgr-autolua" onchange="mgr_save()">
          <label for="mgr-autolua" style="font-weight:600;">Auto Lua on launch</label>
          <input class="inp" id="mgr-autopath" style="flex:1;min-width:160px;font-family:var(--mono);" placeholder="main.lua" onchange="mgr_save()">
          <button class="btn btn-blu" onclick="mgr_run_auto()"><svg class=i><use href=#i-play></use></svg> Test run now</button>
        </div>
      </div>
      <div style="background:var(--bg2);padding:10px 12px;border:1px solid var(--brd);border-radius:var(--rad-md);display:flex;flex-direction:column;gap:6px;">
        <div style="display:flex;gap:8px;flex-wrap:wrap;color:var(--fg-dim);font-size:10px;">
          <span>boots: <b id="mgr-boots" style="color:var(--fg);">0</b></span>
          <span>resumed: <b id="mgr-resumed" style="color:var(--fg);">0</b></span>
          <span>last hold: <b id="mgr-lasthold" style="color:var(--fg);">-</b></span>
          <span>config: <b id="mgr-config" style="color:var(--fg);font-family:var(--mono);">-</b></span>
        </div>
        <div style="color:var(--fg-dim);font-size:10px;text-transform:uppercase;letter-spacing:.5px;">Last Auto Lua output</div>
        <pre id="mgr-output" style="margin:0;background:var(--bg);border:1px solid var(--brd);padding:6px 8px;color:var(--type);font-family:var(--mono);font-size:11px;white-space:pre-wrap;max-height:200px;overflow:auto;">-</pre>
      </div>
    </div>

  </div>
</div>

<div class="modal-overlay" id="wsModal">
  <div class="modal-box" style="width:440px;" onclick="event.stopPropagation()">
    <div class="modal-hdr"><span id="wsModalTitle"></span></div>
    <div class="ws-dialog-msg" id="wsModalMsg"></div>
    <div style="padding:0 14px 10px 14px;"><input class="inp" id="wsModalInput" style="display:none;" autocomplete="off" spellcheck="false"></div>
    <div class="ws-dialog-foot">
      <button class="btn" id="wsModalCancel">Cancel</button>
      <button class="btn btn-grn" id="wsModalOk">OK</button>
    </div>
  </div>
</div>

<div class="modal-overlay" id="addCompModal" onclick="close_add_comp(event)">
  <div class="modal-box" onclick="event.stopPropagation()">
    <div class="modal-hdr">
      <span>Add Component</span>
      <button class="btn btn-red" style="padding:2px 8px;" onclick="close_add_comp()"><svg class=i><use href=#i-x></use></svg></button>
    </div>
    <div style="padding:8px 10px;border-bottom:1px solid var(--brd);"><input class="inp" id="addCompSearch" placeholder="Search component type..." oninput="filter_add_comp()"></div>
    <div class="modal-body" id="addCompCats">Loading...</div>
  </div>
</div>

<div class="modal-overlay" id="dumpModal" onclick="close_dump(event)">
  <div class="modal-box" style="width:900px;height:82vh;" onclick="event.stopPropagation()">
    <div class="modal-hdr">
      <span id="dumpTitle">Dump entire scene</span>
      <div style="display:flex;gap:4px;">
        <button class="btn" id="dump-copy" style="padding:2px 8px;display:none;" onclick="dump_copy()"><svg class=i><use href=#i-copy></use></svg> Copy</button>
        <button class="btn btn-grn" id="dump-dl" style="padding:2px 8px;display:none;" onclick="dump_download()"><svg class=i><use href=#i-download></use></svg> .txt</button>
        <button class="btn btn-org" id="dump-cancel" style="padding:2px 8px;display:none;" onclick="dump_cancel()"><svg class=i><use href=#i-square></use></svg> Cancel</button>
        <button class="btn btn-red" style="padding:2px 8px;" onclick="close_dump()"><svg class=i><use href=#i-x></use></svg></button>
      </div>
    </div>
    <div id="dump-progress" style="padding:6px 12px;color:var(--fg-dim);font-size:11px;"></div>
    <pre class="dump-pre" id="dump-text"></pre>
  </div>
</div>

<div class="modal-overlay" id="callsMethodsModal" onclick="if(event.target===this)this.classList.remove('open')">
  <div class="modal-box" style="width:720px;" onclick="event.stopPropagation()">
    <div class="modal-hdr">
      <span id="callsMethodsTitle">Hooked methods</span>
      <div style="display:flex;gap:4px;">
        <button class="btn" style="padding:2px 8px;" onclick="calls_ignore_all(true)">Ignore all</button>
        <button class="btn" style="padding:2px 8px;" onclick="calls_ignore_all(false)">Unignore all</button>
        <button class="btn btn-red" style="padding:2px 8px;" onclick="document.getElementById('callsMethodsModal').classList.remove('open')"><svg class=i><use href=#i-x></use></svg></button>
      </div>
    </div>
    <div style="padding:6px 10px;"><input class="inp" id="callsMethodsFilter" placeholder="Filter methods..." oninput="calls_render_methods()"></div>
    <div class="modal-body" id="callsMethodsList" style="padding:0;"></div>
  </div>
</div>

<div class="modal-overlay" id="wsPreviewModal" onclick="ws_preview_close()">
  <div class="modal-box" style="width:640px;max-width:92vw;" onclick="event.stopPropagation()">
    <div class="modal-hdr">
      <span id="ws-preview-title" style="font-family:var(--mono);font-size:12px;">preview</span>
      <div style="display:flex;gap:4px;">
        <button class="btn" id="ws-preview-dl" title="Download"><svg class=i><use href=#i-download></use></svg></button>
        <button class="btn btn-red" style="padding:2px 8px;" onclick="ws_preview_close()"><svg class=i><use href=#i-x></use></svg></button>
      </div>
    </div>
    <div id="ws-preview-body" style="padding:16px;background:var(--bg);display:flex;align-items:center;justify-content:center;min-height:200px;"></div>
  </div>
</div>

<div class="modal-overlay" id="collModal" onclick="close_collection(event)">
  <div class="modal-box" onclick="event.stopPropagation()">
    <div class="modal-hdr">
      <span id="collTitle">Collection</span>
      <button class="btn btn-red" style="padding:2px 8px;" onclick="close_collection()"><svg class=i><use href=#i-x></use></svg></button>
    </div>
    <div class="modal-body" id="collList"></div>
  </div>
</div>

<div class="modal-overlay" id="instPickerModal" onclick="close_instance_picker(event)">
  <div class="modal-box" onclick="event.stopPropagation()">
    <div class="modal-hdr">
      <span id="instPickerTitle">Select Instance</span>
      <button class="btn btn-red" style="padding:2px 8px;" onclick="close_instance_picker()"><svg class=i><use href=#i-x></use></svg></button>
    </div>
    <div style="padding:8px 10px;border-bottom:1px solid var(--brd);display:flex;flex-direction:column;gap:6px;">
      <input class="inp" id="instPickerSearch" placeholder="Search by name or address..." oninput="filter_instance_picker()">
      <div style="display:flex;gap:6px;">
        <button class="btn btn-blu" style="flex:1;" onclick="pick_instance('0x0', 'null (0x0)')"><svg class=i><use href=#i-ban></use></svg> Null (0x0)</button>
        <button class="btn btn-grn" style="flex:1;" id="btn-pick-selected-go" onclick="pick_current_selected()"><svg class=i><use href=#i-box></use></svg> Current Object</button>
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
let scene_inspect_generation=0;
let scene_load_generation=0;
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

async function load_instances(){
  document.getElementById('inst-list').innerHTML='Loading...';
  let d=await fetch(`/api/instances?a=${encodeURIComponent(cur_asm)}&ns=${encodeURIComponent(cur_ns)}&n=${encodeURIComponent(cur_cls)}`).then(r=>r.json());
  if(d.error){document.getElementById('inst-list').innerHTML=`<div style="color:var(--mth);padding:12px;">${esc(d.error)}</div>`;return;}
  let h=`<div style="display:flex;justify-content:space-between;align-items:center;margin-bottom:8px;"><span>Found instances: ${d.instances.length}</span><button class="btn btn-blu" onclick="open_static_controller()"><svg class=i><use href=#i-gamepad-2></use></svg> Open Static</button></div><table class="table"><tr><th>Address</th><th>Name</th><th>Action</th></tr>`;
  for(let i of d.instances)
    h+=`<tr><td><span class="c-nm">0x${i.addr}</span></td><td><span class="c-st">"${esc(i.name)}"</span></td><td><button class="btn btn-blu" onclick="open_controller('0x${i.addr}')"><svg class=i><use href=#i-mouse-pointer-click></use></svg> Open</button></td></tr>`;
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

async function load_scene(manual_reload){
  const requestId=++scene_load_generation;
  try{
    let ls=await fetch('/api/scene').then(r=>r.json());
    if(requestId!==scene_load_generation||!Array.isArray(ls))return;
    full_scene_data=ls;
    live_scene_sig=scene_signature(full_scene_data);
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

let insp_sig='';
let live_scene_sig='';
let live_busy=false;
const live_interval_ms=4000;

function insp_signature(d){
  return (d.addr||'')+'|'+(d.scripts||[]).map(s=>s.addr).join(',');
}

function scene_signature(list){
  let parts=new Array(list.length);
  for(let i=0;i<list.length;i++){let g=list[i];parts[i]=g.addr+':'+g.parent+':'+(g.active?1:0)+':'+g.name;}
  parts.sort();
  return parts.join('|');
}

function toggle_live_sync(){
  live_sync_active=!live_sync_active;
  let btn=document.getElementById('btn-live-sync');
  let dot=document.getElementById('live-dot');
  if(live_sync_active){
    btn.classList.add('active');
    dot.classList.remove('off');
    live_schedule(0);
  }else{
    btn.classList.remove('active');
    dot.classList.add('off');
    if(live_sync_timer){clearTimeout(live_sync_timer);live_sync_timer=null;}
  }
}

function live_schedule(ms){
  if(live_sync_timer)clearTimeout(live_sync_timer);
  live_sync_timer=setTimeout(live_tick,ms);
}

async function live_tick(){
  live_sync_timer=null;
  if(!live_sync_active)return;
  if(live_busy){live_schedule(live_interval_ms);return;}
  live_busy=true;
  try{
    let list=await fetch('/api/scene').then(r=>r.json());
    if(Array.isArray(list)){
      let sig=scene_signature(list);
      if(sig!==live_scene_sig){
        live_scene_sig=sig;
        full_scene_data=list;
        let tw=document.getElementById('scene-tree');
        let st=tw?tw.scrollTop:0;
        filter_scene();
        if(tw)tw.scrollTop=st;
      }
    }
    const inspected_addr=selected_go_addr;
    if(inspected_addr){
      let d=await fetch(`/api/scene/inspect?addr=${inspected_addr}`).then(r=>r.json()).catch(()=>null);
      if(selected_go_addr===inspected_addr&&d&&!d.error){
        if(d.stale||!d.addr){
          document.getElementById('scene-insp').innerHTML='<div style="padding:30px;text-align:center;color:var(--mth);">Object is no longer active in scene</div>';
          selected_go_addr=null;
          insp_sig='';
        }else if(insp_signature(d)!==insp_sig){
          let insp=document.getElementById('scene-insp');
          let ae=document.activeElement;
          if(!(ae&&insp.contains(ae)&&(ae.tagName==='INPUT'))){
            let st=insp.scrollTop;
            await inspect_go(inspected_addr);
            if(selected_go_addr===inspected_addr)insp.scrollTop=st;
          }
        }else{
          live_patch_inspector(d);
        }
      }
    }
  }catch(e){}
  live_busy=false;
  if(live_sync_active)live_schedule(live_interval_ms);
}

function live_color_hex(v){
  if(typeof v==='string'){try{v=JSON.parse(v);}catch(e){return null;}}
  if(!Array.isArray(v))return null;
  return '#'+[0,1,2].map(i=>Math.max(0,Math.min(255,Math.round((v[i]||0)*255))).toString(16).padStart(2,'0')).join('');
}

function live_vals(v,t){
  if(typeof v==='string'){try{v=JSON.parse(v);}catch(e){}}
  if(Array.isArray(v))return v;
  if(v&&typeof v==='object'){
    if(String(t).includes('Rect'))return [v.x,v.y,v.w,v.h];
    return [v.x,v.y,v.z,v.w];
  }
  return [v];
}

function live_set_input(el,v){
  if(document.activeElement===el)return;
  if(el.type==='checkbox'){el.checked=(v===true||v==='true');return;}
  if(el.type==='color'){let hx=live_color_hex(v);if(hx&&el.value!==hx)el.value=hx;return;}
  let s=(v===null||v===undefined)?'':String(v).replace(/^"|"$/g,'');
  if(el.value!==s)el.value=s;
}

function live_patch_inspector(d){
  let insp=document.getElementById('scene-insp');
  if(!insp)return;
  let hdr=insp.querySelector('.insp-hdr');
  if(hdr){
    let cb=hdr.querySelector('input[type=checkbox]');
    if(cb&&document.activeElement!==cb)cb.checked=!!d.active;
    let nm=hdr.querySelector('input.inp');
    if(nm&&document.activeElement!==nm&&nm.value!==d.name)nm.value=d.name||'';
  }
  if(d.transform)update_transform_inputs_live(d.transform);
  let map={};
  insp.querySelectorAll('[data-tt="script"]').forEach(el=>{
    let k=el.dataset.addr+'\u0000'+el.dataset.pid;
    (map[k]=map[k]||[]).push(el);
  });
  for(let sc of d.scripts||[]){
    let ecb=insp.querySelector(`input[data-comp="${sc.addr}"]`);
    if(ecb&&document.activeElement!==ecb)ecb.checked=!!sc.enabled;
    for(let r of sc.fields||[]){
      let els=map[sc.addr+'\u0000'+r.name];
      if(!els)continue;
      if(els.length===1)live_set_input(els[0],r.val);
      else{
        let vals=live_vals(r.val,r.type);
        els.forEach((el,i)=>live_set_input(el,vals[i]));
      }
    }
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
  let map=Object.create(null);
  let nodes=[];
  scene_roots=[];
  for(let g of full_scene_data){
    if(!g||!g.addr)continue;
    let node=map[g.addr];
    if(!node){node={...g,children:[]};map[g.addr]=node;nodes.push(node);}
    else Object.assign(node,g);
  }
  let linked=new Set();
  for(let g of full_scene_data){
    if(!g||!g.addr)continue;
    let node=map[g.addr];
    let parent=(g.parent&&g.parent!=="0"&&g.parent!==0)?map[g.parent]:null;
    if(parent&&parent!==node){
      parent.children.push(node);
      linked.add(g.addr);
    }else{
      scene_roots.push(node);
    }
  }
  let visited=new Set();
  function mark_scene_seen(node,path){
    if(!node||visited.has(node.addr)||path.has(node.addr))return;
    visited.add(node.addr);
    let next=new Set(path);
    next.add(node.addr);
    for(let child of node.children||[])mark_scene_seen(child,next);
  }
  for(let root of scene_roots)mark_scene_seen(root,new Set());
  for(let node of nodes){
    if(!visited.has(node.addr)){
      scene_roots.push(node);
      mark_scene_seen(node,new Set());
    }
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

function build_scene_node(n,q,path=new Set()){
  if(!n||path.has(n.addr))return'';
  let next=new Set(path);
  next.add(n.addr);
  let matches=!q||n.name?.toLowerCase().includes(q);
  let child_html=(n.children||[]).map(c=>build_scene_node(c,q,next)).filter(Boolean).join('');
  if(!matches&&!child_html)return'';
  let has_kids=(n.children||[]).length>0;
  let is_open=open_scene_nodes.has(n.addr)||(q&&(matches||child_html.length>0));
  return`<ul class="tree-node"><div class="tree-nc" data-go="${n.addr}" onclick="inspect_go('${n.addr}')"><span class="arr${is_open?' open':''}" onclick="event.stopPropagation();toggle_scene_expand('${n.addr}',this)">${has_kids?'<svg class=i><use href=#i-chevron-right></use></svg>':''}</span><div class="ico"><svg class=i><use href=#i-box></use></svg></div><span style="color:${n.active?'var(--fg)':'#7a7a7a'}">${esc(n.name)}</span></div>${has_kids?`<div class="kids${is_open?' open':''}">${child_html}</div>`:''}</ul>`;
}

async function inspect_go(addr){
  const requestId=++scene_inspect_generation;
  document.querySelectorAll('#scene-tree .tree-nc').forEach(e=>e.classList.remove('active'));
  let el=document.querySelector(`[data-go="${addr}"]`);
  if(el)el.classList.add('active');
  selected_go_addr=addr;
  let d=await fetch(`/api/scene/inspect?addr=${addr}`).then(r=>r.json()).catch(()=>({error:'request failed'}));
  if(requestId!==scene_inspect_generation||selected_go_addr!==addr)return;
  if(d.error){
    let msg=d.error==='timeout'?'Game thread is busy, tap the object again':esc(d.error);
    document.getElementById('scene-insp').innerHTML=`<div style="padding:30px;text-align:center;color:var(--mth);">${msg}</div>`;
    return;
  }
  if(d.stale||!d.addr){
    document.getElementById('scene-insp').innerHTML='<div style="padding:30px;text-align:center;color:var(--mth);">Object is no longer active in scene</div>';
    selected_go_addr=null;
    return;
  }
  selected_go_name=d.name||"GameObject";
  insp_sig=insp_signature(d);
  let h=`<div class="insp-hdr">
    <input type="checkbox" ${d.active?'checked':''} onchange="update_go('${d.addr}','gameobject','active',this.checked)">
    <div class="ico" style="color:var(--type)"><svg class=i><use href=#i-box></use></svg></div>
    <input type="text" class="inp" value="${esc(d.name)}" onchange="update_go('${d.addr}','gameobject','name',this.value)">
    <button class="btn btn-grn" style="padding:4px 8px;" onclick="open_add_comp('${d.addr}')" title="Add Component"><svg class=i><use href=#i-plus></use></svg> Add</button>
    <button class="btn btn-red" style="padding:4px 8px;" onclick="delete_go('${d.addr}')" title="Delete GameObject"><svg class=i><use href=#i-trash-2></use></svg></button>
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
  let result=await fetch('/api/scene/delete',{method:'POST',headers:{'Content-Type':'application/json'},body:JSON.stringify({addr})}).then(r=>r.json()).catch(()=>({ok:false,error:'request failed'}));
  if(!result.ok){alert('Delete failed: '+(result.error||'unknown error'));return;}
  scene_inspect_generation++;
  document.getElementById('scene-insp').innerHTML='<div style="padding:30px;text-align:center;color:var(--fg-dim);">Destroy scheduled</div>';
  selected_go_addr=null;
  await new Promise(resolve=>setTimeout(resolve,350));
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
  let result=await fetch('/api/scene/removecomponent',{method:'POST',headers:{'Content-Type':'application/json'},body:JSON.stringify({addr:comp_addr})}).then(r=>r.json()).catch(()=>({ok:false,error:'request failed'}));
  if(!result.ok){alert('Remove failed: '+(result.error||'unknown error'));return;}
  await new Promise(resolve=>setTimeout(resolve,350));
  if(selected_go_addr===go_addr)inspect_go(go_addr);
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
    document.getElementById('collList').innerHTML=`<div style="padding:20px;text-align:center;color:var(--mth);">${esc(d&&d.error?d.error:'Failed to read collection')}</div>`;
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
    h+=`<div class="coll-row${clk}"><span class="coll-idx">${esc(it.i)}</span><span class="coll-val" title="${esc(it.v)}">${esc(it.v)}</span><span class="coll-ty">${esc(it.t)}</span>${it.addr?'<svg class=i><use href=#i-chevron-right></use></svg>':''}</div>`;
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
    document.getElementById('instPickerList').innerHTML='<div style="padding:20px;text-align:center;color:var(--mth);">Failed to query instances</div>';
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
      <button class="btn btn-blu" style="padding:2px 8px;"><svg class=i><use href=#i-check></use></svg> Select</button>
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
    <input type="checkbox" data-comp="${sc.addr}" ${sc.enabled?'checked':''} onchange="update_script_enabled('${sc.addr}',this.checked)" onclick="event.stopPropagation()" style="margin-right:4px;">
    <div class="ico" style="color:${col}">${ic(is_builtin?'layers':'terminal')}</div>
    <span style="color:${col}">${esc(sc.name)}</span>
    ${sc.ns?`<span style="opacity:.6;font-size:10px;margin-left:4px;">${esc(sc.ns)}</span>`:''}
    <span style="opacity:.4;font-size:10px;margin-left:auto;margin-right:4px;">0x${sc.addr}</span>
    <button class="btn btn-red" style="padding:2px 6px;" title="Remove" onclick="event.stopPropagation();remove_component('${sc.addr}','${go_addr}')"><svg class=i><use href=#i-trash-2></use></svg></button>
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
  let h=`<div class="comp"><div class="comp-hdr" onclick="toggle_comp('${uid}')"><div class="ico"><svg class=i><use href=#i-layers></use></svg></div><span>${esc(title)}</span></div><div class="comp-body open" id="${uid}">`;
  for(let r of rows){
    let da=`data-addr="${target_addr}" data-tt="${target_type}" data-pid="${r.id||r.name||r.lbl}" data-vt="${r.type}" data-ip="${r.isProp?'true':'false'}" data-static="${r.static?'true':'false'}"`;
    let dis=r.canWrite===false?'disabled':'';
    h+=build_field_row(r,da,dis,with_save,target_type==='instance');
  }
  return h+'</div></div>';
}

function build_field_row(r,da,dis,with_save,show_loop){
  let label=r.name||r.lbl||r.id||'';
  let prefix=r.static?'<span style="color:var(--str);font-size:10px;font-weight:700;">[Static] </span>':'';
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
      h+=`<button class="btn" style="padding:2px 7px;" onclick="open_object_ref('${oref.addr}')" title="${oref.is_list?'Show items':'Open object'}">${ic(oref.is_list?'list':'external-link')}</button>`;
    }
    if(is_obj&&!dis){
      h+=`<button class="btn btn-blu" style="padding:2px 7px;" onclick="open_instance_picker('${esc(t)}','${row_id}')" title="Pick live instance"><svg class=i><use href=#i-mouse-pointer-click></use></svg></button>`;
    }
  }

  if(with_save&&!dis){
    h+=`<button class="btn btn-grn" style="margin-left:4px;padding:3px 8px;" onclick="handle_save(this)"><svg class=i><use href=#i-check></use></svg> Set</button>`;
    if(show_loop)h+=`<button class="btn btn-org" style="padding:3px 7px;" title="Loop" onclick="prompt_loop(this)"><svg class=i><use href=#i-repeat></use></svg></button>`;
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
      <svg class=i><use href=#i-repeat></use></svg>
      <span class="loop-id">${esc(lp.cls)}.${esc(lp.name)}</span>
      <span style="color:var(--str);font-family:Consolas,monospace;font-size:11px;">${esc(lp.val)}</span>
      <span style="color:var(--fg-dim);font-size:10px;">${lp.interval}ms</span>
      <button class="btn btn-red" style="padding:2px 8px;" onclick="remove_loop('${esc(lp.id)}',this)"><svg class=i><use href=#i-x></use></svg> Stop</button>
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

function download_dump(){
  let a=document.createElement('a');
  a.href='/api/dump';a.download='dump.cs';a.click();
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

let dump_timer=null;
async function open_dump(){
  document.getElementById('dumpModal').classList.add('open');
  document.getElementById('dump-text').textContent='';
  ['dump-copy','dump-dl'].forEach(i=>document.getElementById(i).style.display='none');
  document.getElementById('dump-cancel').style.display='inline-flex';
  document.getElementById('dump-progress').textContent='Starting...';
  let r=await fetch('/api/scene/dump/start',{method:'POST'}).then(r=>r.json()).catch(()=>null);
  if(!r||(!r.ok&&r.error!=='a dump is already running')){
    document.getElementById('dump-progress').textContent='Failed: '+(r&&r.error?r.error:'no response');
    document.getElementById('dump-cancel').style.display='none';
    return;
  }
  if(dump_timer)clearInterval(dump_timer);
  dump_timer=setInterval(dump_poll,700);
}

async function dump_poll(){
  let s=await fetch('/api/scene/dump/status').then(r=>r.json()).catch(()=>null);
  if(!s)return;
  let pr=document.getElementById('dump-progress');
  if(s.running){
    pr.textContent=`Dumping ${s.processed} / ${s.total} objects, ${(s.size/1024).toFixed(0)} KB`;
    return;
  }
  if(!s.done)return;
  clearInterval(dump_timer);dump_timer=null;
  document.getElementById('dump-cancel').style.display='none';
  pr.textContent=`Done: ${s.processed} objects, ${(s.size/1024).toFixed(0)} KB${s.error?' ('+s.error+')':''}`;
  let t=await fetch('/api/scene/dump/result').then(r=>r.text()).catch(()=>'');
  let shown=t.length>600000?t.slice(0,600000)+'\n... preview truncated, use .txt to get everything':t;
  document.getElementById('dump-text').textContent=shown;
  document.getElementById('dump-copy').style.display='inline-flex';
  document.getElementById('dump-dl').style.display='inline-flex';
}

async function dump_cancel(){
  await fetch('/api/scene/dump/cancel',{method:'POST'}).catch(()=>{});
}

function dump_download(){
  let a=document.createElement('a');
  a.href='/api/scene/dump/result?download=1';a.download='scene_dump.txt';a.click();
}

async function dump_copy(){
  let t=await fetch('/api/scene/dump/result').then(r=>r.text()).catch(()=>'');
  try{await navigator.clipboard.writeText(t);}catch(e){
    let ta=document.createElement('textarea');ta.value=t;document.body.appendChild(ta);ta.select();document.execCommand('copy');ta.remove();
  }
}

function close_dump(e){
  if(!e||e.target===document.getElementById('dumpModal')){
    document.getElementById('dumpModal').classList.remove('open');
  }
}

async function refresh_freeze(){
  let d=await fetch('/api/freeze').then(r=>r.json()).catch(()=>null);
  if(d)apply_freeze_ui(d.frozen,d);
}

function apply_freeze_ui(on,d){
  let b=document.getElementById('btn-freeze');
  let l=document.getElementById('freeze-lbl');
  if(!b)return;
  b.classList.toggle('active',!!on);
  l.textContent=on?`Frozen (${(d&&d.blocked_destroy||0)+(d&&d.blocked_scene||0)})`:'Freeze';
}

async function toggle_freeze(){
  let cur=document.getElementById('btn-freeze').classList.contains('active');
  let d=await fetch('/api/freeze',{method:'POST',headers:{'Content-Type':'application/json'},body:JSON.stringify({on:!cur})}).then(r=>r.json()).catch(()=>null);
  if(!d){alert('Freeze request failed');return;}
  if(!d.ok){alert('Freeze failed: '+(d.error||'unknown'));return;}
  apply_freeze_ui(d.frozen,d);
}
setInterval(()=>{if(document.getElementById('btn-freeze')&&document.getElementById('btn-freeze').classList.contains('active'))refresh_freeze();},2000);
refresh_freeze();

let calls_seq=0;
let calls_items=[];
let calls_methods=[];
let calls_timer=null;
let calls_paused=false;
const calls_max_ui=4000;

function calls_on_show(){
  calls_poll_status();
  if(!calls_timer)calls_timer=setInterval(calls_tick,800);
}

async function calls_tick(){
  if(!document.getElementById('pnl-calls').classList.contains('active')){clearInterval(calls_timer);calls_timer=null;return;}
  try{
    let d=await fetch('/api/calls/log?since='+calls_seq).then(r=>r.json());
    if(d.latest<calls_seq){calls_items=[];calls_seq=0;}
    if(d.items&&d.items.length){
      calls_items=calls_items.concat(d.items);
      if(calls_items.length>calls_max_ui)calls_items=calls_items.slice(calls_items.length-calls_max_ui);
      calls_seq=d.seq;
      calls_render();
    }else if(d.latest===0&&calls_items.length){calls_items=[];calls_seq=0;calls_render();}
    calls_poll_status();
  }catch(e){}
}

async function calls_poll_status(){
  let s=await fetch('/api/calls/status').then(r=>r.json()).catch(()=>null);
  if(!s)return;
  calls_paused=s.paused;
  let pb=document.getElementById('calls-pause-btn');
  pb.innerHTML=s.paused?'<svg class=i><use href=#i-play></use></svg> Resume':'<svg class=i><use href=#i-pause></use></svg> Pause';
  calls_scene_on=!!(s.scene&&s.scene.on);
  let sb=document.getElementById('calls-scene-btn');
  if(sb)sb.innerHTML=calls_scene_on?'<svg class=i><use href=#i-eye-off></use></svg> Stop scene watch':'<svg class=i><use href=#i-eye></use></svg> Watch scene';
  let st=`${s.hooked} methods hooked`;
  if(s.scene&&s.scene.on)st+=` | scene: ${s.scene.classes} classes, ${s.scene.queued} queued, ${s.scene.failed} skipped`;
  if(s.scene&&s.scene.main_thread_error)st+=' | '+esc(s.scene.main_thread_error)+' (hooking from a background thread)';
  if(s.targets&&s.targets.length)st+=' | '+s.targets.map(t=>`${esc(t.label)} (${t.count}) <a href="#" style="color:var(--mth)" onclick="calls_unwatch(${t.id});return false;">x</a>`).join(', ');
  if(s.job&&s.job.running)st+=` | hooking ${s.job.done}/${s.job.total} (ok ${s.job.hooked}, failed ${s.job.failed})`;
  else if(s.job&&s.job.error)st+=' | '+esc(s.job.error);
  document.getElementById('calls-status').innerHTML=st;
}

async function calls_watch(){
  let ns=document.getElementById('calls-ns').value.trim();
  let cls=document.getElementById('calls-cls').value.trim();
  if(!cls){alert('Enter a class name');return;}
  let r=await fetch('/api/calls/watch',{method:'POST',headers:{'Content-Type':'application/json'},body:JSON.stringify({asm:'',ns,cls})}).then(r=>r.json()).catch(()=>({error:'request failed'}));
  if(r.error)alert(r.error);
  calls_on_show();
}

let calls_scene_on=false;
async function calls_toggle_scene(){
  let on=!calls_scene_on;
  let auto=document.getElementById('calls-auto').checked;
  let r=await fetch('/api/calls/scene',{method:'POST',headers:{'Content-Type':'application/json'},body:JSON.stringify({on,auto})}).then(r=>r.json()).catch(()=>null);
  if(!r){alert('Request failed');return;}
  calls_on_show();
  calls_poll_status();
}

async function calls_unwatch(target){
  await fetch('/api/calls/unwatch',{method:'POST',headers:{'Content-Type':'application/json'},body:JSON.stringify({target})});
  calls_poll_status();
}

async function calls_toggle_pause(){
  let r=await fetch('/api/calls/pause',{method:'POST',headers:{'Content-Type':'application/json'},body:JSON.stringify({paused:!calls_paused})}).then(r=>r.json());
  calls_paused=r.paused;calls_poll_status();
}

async function calls_clear(){
  await fetch('/api/calls/clear',{method:'POST'});
  calls_items=[];calls_seq=0;calls_render();
}

async function calls_config(){
  let lim=parseInt(document.getElementById('calls-limit').value);
  if(isNaN(lim))return;
  await fetch('/api/calls/config',{method:'POST',headers:{'Content-Type':'application/json'},body:JSON.stringify({limit:lim})});
}

async function calls_ignore_one(id,ignored){
  await fetch('/api/calls/ignore',{method:'POST',headers:{'Content-Type':'application/json'},body:JSON.stringify({ids:[id],ignored})});
  if(document.getElementById('callsMethodsModal').classList.contains('open'))calls_show_methods();
}

async function calls_ignore_all(ignored){
  await fetch('/api/calls/ignore',{method:'POST',headers:{'Content-Type':'application/json'},body:JSON.stringify({all:true,ignored})});
  calls_show_methods();
}

async function calls_show_methods(){
  document.getElementById('callsMethodsModal').classList.add('open');
  let d=await fetch('/api/calls/methods').then(r=>r.json()).catch(()=>null);
  calls_methods=d?d.methods:[];
  document.getElementById('callsMethodsTitle').textContent='Hooked methods ('+(d?d.total:0)+')';
  calls_render_methods();
}

function calls_render_methods(){
  let q=(document.getElementById('callsMethodsFilter').value||'').toLowerCase();
  let list=calls_methods.filter(m=>!q||(m.c+'::'+m.f).toLowerCase().includes(q)).slice(0,600);
  document.getElementById('callsMethodsList').innerHTML=list.map(m=>`<div class="calls-m${m.i?' ign':''}">
    <input type="checkbox" ${m.i?'':'checked'} title="Record this method" onchange="calls_ignore_one(${m.id},!this.checked)">
    <span style="flex:1;overflow:hidden;text-overflow:ellipsis;"><span style="color:var(--type)">${esc(m.c)}</span>::<span style="color:var(--mth);font-weight:700;">${esc(m.f)}</span>(${esc(m.p)})</span>
    <span style="color:var(--num)">${m.n}${m.d?` <span style="color:var(--mth)">(-${m.d})</span>`:''}</span></div>`).join('')||'<div style="padding:20px;text-align:center;color:var(--fg-dim);">No hooked methods</div>';
}




const ws={entries:[],open:[],active:null,buf:{},expanded:{},sel_dir:'',side:'files',info:null,ready:false,zip_name:'workspace'};

function ws_esc(t){return String(t).replace(/&/g,'&amp;').replace(/</g,'&lt;').replace(/>/g,'&gt;').replace(/"/g,'&quot;');}
function ws_log(t){let o=document.getElementById('lua-output');o.textContent+=t+'\n';o.scrollTop=o.scrollHeight;}

async function ws_api(url,opts){
  let r=await fetch(url,opts);
  let d=null;
  try{d=await r.json();}catch(e){}
  if(!r.ok||(d&&d.error))throw new Error((d&&d.error)||('HTTP '+r.status));
  return d;
}
function ws_post(url,obj){return ws_api(url,{method:'POST',headers:{'Content-Type':'application/json'},body:JSON.stringify(obj)});}

function ws_dialog(o){
  return new Promise(resolve=>{
    const m=document.getElementById('wsModal');
    const inp=document.getElementById('wsModalInput');
    const ok=document.getElementById('wsModalOk');
    const cancel=document.getElementById('wsModalCancel');
    document.getElementById('wsModalTitle').textContent=o.title||'';
    document.getElementById('wsModalMsg').textContent=o.message||'';
    inp.style.display=o.input!==undefined?'block':'none';
    inp.value=o.input||'';
    ok.textContent=o.ok||'OK';
    ok.className='btn '+(o.danger?'btn-red':'btn-grn');
    cancel.style.display=o.alert?'none':'';
    function done(v){m.classList.remove('open');ok.onclick=null;cancel.onclick=null;inp.onkeydown=null;m.onclick=null;resolve(v);}
    ok.onclick=()=>done({ok:true,value:inp.value});
    cancel.onclick=()=>done({ok:false});
    m.onclick=e=>{if(e.target===m)done({ok:false});};
    inp.onkeydown=e=>{if(e.key==='Enter')done({ok:true,value:inp.value});if(e.key==='Escape')done({ok:false});};
    m.classList.add('open');
    if(o.input!==undefined)setTimeout(()=>{inp.focus();inp.select();},40);
  });
}

function ws_toggle_sidebar(){document.getElementById('ws-side').classList.toggle('show');}

function ws_side_tab(t){
  ws.side=t;
  document.getElementById('ws-stab-files').classList.toggle('active',t==='files');
  document.getElementById('ws-stab-zips').classList.toggle('active',t==='zips');
  document.getElementById('ws-files-pane').style.display=t==='files'?'flex':'none';
  document.getElementById('ws-zips-pane').style.display=t==='zips'?'flex':'none';
  if(t==='zips')ws_refresh_zips();
}

async function ws_on_show(){
  if(ws.ready)return;
  try{
    ws.info=await ws_api('/api/ws/info');
    await ws_refresh();
    ws.ready=true;
    if(ws.entries.some(e=>!e.dir&&e.path==='main.lua'))await ws_open_file('main.lua');
    else ws_render_editor();
  }catch(e){ws_log('Workspace error: '+e.message);}
}

async function ws_refresh(){
  let d=await ws_api('/api/ws/tree');
  ws.entries=d.entries||[];
  ws_render_tree();
}

function ws_build_tree(){
  const root={dirs:{},files:[],path:''};
  for(const e of ws.entries){
    const parts=e.path.split('/');
    const depth=e.dir?parts.length:parts.length-1;
    let node=root;
    for(let i=0;i<depth;i++){
      const p=parts[i];
      if(!node.dirs[p])node.dirs[p]={dirs:{},files:[],path:parts.slice(0,i+1).join('/')};
      node=node.dirs[p];
    }
    if(!e.dir)node.files.push({name:parts[parts.length-1],path:e.path,lua:e.lua!==false&&/\.lua$/i.test(e.path)});
  }
  return root;
}

function ws_render_tree(){
  const el=document.getElementById('ws-tree');
  const root=ws_build_tree();
  let html='';
  function walk(node,depth){
    for(const dn of Object.keys(node.dirs).sort()){
      const d=node.dirs[dn];
      const open=ws.expanded[d.path]!==false;
      html+=`<div class="ws-row${ws.sel_dir===d.path?' active':''}" style="padding-left:${6+depth*14}px" data-path="${ws_esc(d.path)}" data-dir="1" onclick="ws_row_click(this)">`+
        `${ic(open?'folder-open':'folder')}<span class="nm">${ws_esc(dn)}</span>`+
        `<span class="acts"><button class="ws-ibtn" title="Rename" onclick="event.stopPropagation();ws_rename(this.closest('.ws-row').dataset.path)"><svg class=i><use href=#i-pencil></use></svg></button>`+
        `<button class="ws-ibtn" title="Delete" onclick="event.stopPropagation();ws_delete(this.closest('.ws-row').dataset.path,true)"><svg class=i><use href=#i-trash-2></use></svg></button></span></div>`;
      if(open)walk(d,depth+1);
    }
    for(const f of node.files.sort((a,b)=>a.name.localeCompare(b.name))){
      const isLua=f.lua;
      const kind=ws_file_kind(f.path);
      const icon=isLua?'i-file-text':kind==='image'?'i-image':kind==='audio'?'i-music':kind==='text'?'i-file-text':'i-package';
      const editable=isLua||kind==='text';
      const viewBtn=(kind==='image'||kind==='audio')
        ?`<button class="ws-ibtn" title="Open" onclick="event.stopPropagation();ws_view_file(this.closest('.ws-row').dataset.path)"><svg class=i><use href=#i-eye></use></svg></button>`
        :'';
      html+=`<div class="ws-row${ws.active===f.path?' active':''}" style="padding-left:${6+depth*14}px" data-path="${ws_esc(f.path)}" data-lua="${isLua?1:0}" onclick="ws_row_click(this)">`+
        `<svg class=i><use href=#${icon}></use></svg><span class="nm">${ws_esc(f.name)}</span>`+
        `<span class="acts">`+
        (editable
          ?`<button class="ws-ibtn" title="Rename" onclick="event.stopPropagation();ws_rename(this.closest('.ws-row').dataset.path)"><svg class=i><use href=#i-pencil></use></svg></button>`
          :'')+
        viewBtn+
        (editable?'':`<button class="ws-ibtn" title="Download" onclick="event.stopPropagation();ws_download(this.closest('.ws-row').dataset.path)"><svg class=i><use href=#i-download></use></svg></button>`)+
        `<button class="ws-ibtn" title="Delete" onclick="event.stopPropagation();ws_delete(this.closest('.ws-row').dataset.path,false)"><svg class=i><use href=#i-trash-2></use></svg></button></span></div>`;
    }
  }
  walk(root,0);
  el.innerHTML=html||'<div class="ws-hint">Workspace is empty. Create a file with the button above.</div>';
}

function ws_download(path){
  const a=document.createElement('a');
  a.href='/api/ws/raw?path='+encodeURIComponent(path);
  a.download=path.split('/').pop();
  document.body.appendChild(a);
  a.click();
  a.remove();
}

const ws_image_ext=new Set(['png','jpg','jpeg','gif','webp','bmp','svg']);
const ws_audio_ext=new Set(['wav','ogg','oga','mp3','m4a','aac','flac']);
const ws_text_ext=new Set(['txt','log','md','json','csv','xml','ini','cfg','conf','yaml','yml']);

function ws_file_ext(path){
  const n=path.split('/').pop();
  const i=n.lastIndexOf('.');
  return i>=0?n.slice(i+1).toLowerCase():'';
}

function ws_file_kind(path){
  const e=ws_file_ext(path);
  if(ws_image_ext.has(e))return'image';
  if(ws_audio_ext.has(e))return'audio';
  if(ws_text_ext.has(e))return'text';
  return null;
}

function ws_preview_close(){
  document.getElementById('wsPreviewModal').classList.remove('open');
  document.getElementById('ws-preview-body').innerHTML='';
}

function ws_view_file(path){
  const kind=ws_file_kind(path);
  const name=path.split('/').pop();
  const url='/api/ws/raw?view=1&path='+encodeURIComponent(path);
  const box=document.getElementById('ws-preview-body');
  if(kind==='image')
    box.innerHTML='<img src="'+url+'" alt="'+ws_esc(name)+'" style="max-width:100%;max-height:60vh;border-radius:8px;">';
  else
    box.innerHTML='<div style="width:100%;padding:10px 0;"><div style="text-align:center;margin-bottom:10px;"><svg class=i style="width:34px;height:34px;color:var(--fg-dim);"><use href=#i-music></use></svg></div><audio controls autoplay style="width:100%;" src="'+url+'"></audio></div>';
  document.getElementById('ws-preview-title').textContent=name;
  document.getElementById('ws-preview-dl').onclick=()=>ws_download(path);
  document.getElementById('wsPreviewModal').classList.add('open');
}

function ws_row_click(row){
  const path=row.dataset.path;
  if(row.dataset.dir){
    ws.expanded[path]=ws.expanded[path]===false;
    ws.sel_dir=path;
    ws_render_tree();
    return;
  }
  const i=path.lastIndexOf('/');
  ws.sel_dir=i>=0?path.substring(0,i):'';
  const kind=ws_file_kind(path);
  if(kind==='image'||kind==='audio'){ws_view_file(path);}
  else if(kind==='text'||row.dataset.lua==='1'){ws_open_file(path);}
  else{ws_download(path);}
  if(window.innerWidth<=860)document.getElementById('ws-side').classList.remove('show');
}

async function ws_open_file(path){
  if(!ws.buf[path]){
    try{
      let d=await ws_api('/api/ws/file?path='+encodeURIComponent(path));
      ws.buf[path]={text:d.content,dirty:false};
    }catch(e){ws_log('Open failed: '+e.message);return;}
  }
  if(!ws.open.includes(path))ws.open.push(path);
  ws.active=path;
  ws_render_editor();
  ws_render_tree();
}

function ws_render_tabs(){
  const el=document.getElementById('ws-tabs');
  let html='';
  for(const p of ws.open){
    const name=p.split('/').pop();
    const dirty=ws.buf[p]&&ws.buf[p].dirty;
    html+=`<div class="ws-tab${p===ws.active?' active':''}" data-path="${ws_esc(p)}" title="${ws_esc(p)}" onclick="ws_open_file(this.dataset.path)">`+
      `<span>${ws_esc(name)}</span>${dirty?'<span class="dirty"></span>':''}`+
      `<button class="ws-ibtn" title="Close" onclick="event.stopPropagation();ws_close_file(this.closest('.ws-tab').dataset.path)"><svg class=i><use href=#i-x></use></svg></button></div>`;
  }
  if(ws.active)html+=`<div class="ws-tab" style="margin-left:auto;border-right:none;" title="Run this file" onclick="ws_run(ws.active)"><svg class=i><use href=#i-play></use></svg></div>`;
  el.innerHTML=html;
}

function ws_update_gutter(){
  const ed=document.getElementById('lua-editor');
  const n=ed.value.split('\n').length;
  let t='';
  for(let i=1;i<=n;i++)t+=i+(i<n?'\n':'');
  const g=document.getElementById('ws-gutter');
  g.textContent=t;
  g.scrollTop=ed.scrollTop;
  if(typeof lua_hl_sync==='function')lua_hl_sync();
}

function ws_render_editor(){
  const ed=document.getElementById('lua-editor');
  const empty=document.getElementById('ws-empty');
  ws_render_tabs();
  if(ws.active&&ws.buf[ws.active]){
    ed.disabled=false;
    ed.value=ws.buf[ws.active].text;
    empty.style.display='none';
  }else{
    ed.value='';
    ed.disabled=true;
    empty.style.display='flex';
  }
  ws_update_gutter();
}

async function ws_close_file(path){
  const b=ws.buf[path];
  if(b&&b.dirty){
    const r=await ws_dialog({title:'Unsaved changes',message:path+' has unsaved changes. Close it and discard them?',ok:'Discard',danger:true});
    if(!r.ok)return;
  }
  ws.open=ws.open.filter(p=>p!==path);
  delete ws.buf[path];
  if(ws.active===path)ws.active=ws.open.length?ws.open[ws.open.length-1]:null;
  ws_render_editor();
  ws_render_tree();
}

async function ws_save_file(path){
  const b=ws.buf[path];
  if(!b||!b.dirty)return true;
  try{
    await ws_post('/api/ws/file',{path:path,content:b.text});
    b.dirty=false;
    ws_render_tabs();
    return true;
  }catch(e){ws_log('Save failed ('+path+'): '+e.message);return false;}
}

async function ws_save_all(){
  let ok=true;
  for(const p of Object.keys(ws.buf))if(!(await ws_save_file(p)))ok=false;
  return ok;
}

function ws_child_path(name){
  name=name.trim().replace(/\\/g,'/').replace(/^\/+/,'');
  return ws.sel_dir?ws.sel_dir+'/'+name:name;
}

async function ws_new_file(){
  const r=await ws_dialog({title:'New file',message:ws.sel_dir?'Folder: '+ws.sel_dir:'Folder: workspace root (any extension allowed)',input:'new_file.lua',ok:'Create'});
  if(!r.ok||!r.value.trim())return;
  let name=r.value.trim();
  if(!/\.[A-Za-z0-9]+$/.test(name))name+='.lua';
  const path=ws_child_path(name);
  if(ws.entries.some(e=>e.path===path)){ws_log('File already exists: '+path);return;}
  try{
    let d=await ws_post('/api/ws/file',{path:path,content:''});
    await ws_refresh();
    if(/\.lua$/i.test(d.path))await ws_open_file(d.path);
  }catch(e){ws_log('Create failed: '+e.message);}
}

async function ws_new_folder(){
  const r=await ws_dialog({title:'New folder',message:ws.sel_dir?'Folder: '+ws.sel_dir:'Folder: workspace root',input:'folder',ok:'Create'});
  if(!r.ok||!r.value.trim())return;
  try{
    let d=await ws_post('/api/ws/mkdir',{path:ws_child_path(r.value)});
    ws.expanded[d.path]=true;
    await ws_refresh();
  }catch(e){ws_log('Create failed: '+e.message);}
}

function ws_upload_click(){
  document.getElementById('ws-upload-input').click();
}

async function ws_upload_selected(inp){
  const f=inp.files&&inp.files[0];
  inp.value='';
  if(!f)return;
  const path=ws_child_path(f.name);
  try{
    const buf=await f.arrayBuffer();
    if(buf.byteLength>1024*1024){ws_log('Upload failed: file exceeds 1 MB limit');return;}
    let d=await fetch('/api/ws/upload?path='+encodeURIComponent(path),{method:'POST',headers:{'Content-Type':'application/octet-stream'},body:buf}).then(r=>r.json());
    if(d.error){ws_log('Upload failed: '+d.error);return;}
    ws_log('> Uploaded '+d.path+' ('+buf.byteLength+' bytes)');
    await ws_refresh();
  }catch(e){ws_log('Upload failed: '+e.message);}
}

function ws_remap(from,to){
  const map=p=>p===from?to:(p.startsWith(from+'/')?to+p.substring(from.length):p);
  const nb={};
  for(const p of Object.keys(ws.buf))nb[map(p)]=ws.buf[p];
  ws.buf=nb;
  ws.open=ws.open.map(map);
  if(ws.active)ws.active=map(ws.active);
  if(ws.sel_dir)ws.sel_dir=map(ws.sel_dir);
}

async function ws_rename(path){
  const r=await ws_dialog({title:'Rename',message:path,input:path,ok:'Rename'});
  if(!r.ok||!r.value.trim()||r.value.trim()===path)return;
  let target=r.value.trim();
  const isFile=ws.entries.some(e=>e.path===path&&!e.dir);
  if(isFile&&!/\.lua$/i.test(target))target+='.lua';
  try{
    let d=await ws_post('/api/ws/rename',{from:path,to:target});
    ws_remap(path,d.path);
    await ws_refresh();
    ws_render_editor();
  }catch(e){ws_log('Rename failed: '+e.message);}
}

async function ws_delete(path,is_dir){
  const r=await ws_dialog({title:'Delete',message:(is_dir?'Delete folder ':'Delete file ')+path+(is_dir?' and everything inside it?':'?'),ok:'Delete',danger:true});
  if(!r.ok)return;
  try{
    await ws_post('/api/ws/delete',{path:path});
    const gone=p=>p===path||p.startsWith(path+'/');
    ws.open=ws.open.filter(p=>!gone(p));
    for(const p of Object.keys(ws.buf))if(gone(p))delete ws.buf[p];
    if(ws.active&&gone(ws.active))ws.active=ws.open.length?ws.open[ws.open.length-1]:null;
    if(gone(ws.sel_dir))ws.sel_dir='';
    await ws_refresh();
    ws_render_editor();
  }catch(e){ws_log('Delete failed: '+e.message);}
}

let ws_run_main=true;
async function ws_run(path){
  path=path||'main.lua';
  if(!(await ws_save_all()))return;
  ws_log('> Running '+path+(ws_run_main?' (main thread)':''));
  try{
    let d=await ws_post('/api/ws/run',{path:path,main:ws_run_main});
    if(d.output)ws_log(d.output);
  }catch(e){ws_log('Error: '+e.message);}
}

async function ws_reset_after_replace(){
  ws.open=[];ws.buf={};ws.active=null;ws.sel_dir='';ws.expanded={};
  await ws_refresh();
  if(ws.entries.some(e=>!e.dir&&e.path==='main.lua'))await ws_open_file('main.lua');
  else ws_render_editor();
}

const ws_replace_warning='This replaces everything in the current workspace. All existing .lua files will be deleted and cannot be restored. Save your work first (Save On Device or Export ZIP) before continuing.';

async function ws_import_click(){
  const r=await ws_dialog({title:'Import ZIP',message:ws_replace_warning,ok:'OK, import',danger:true});
  if(!r.ok)return;
  document.getElementById('ws-file-input').click();
}

async function ws_import_selected(inp){
  const f=inp.files&&inp.files[0];
  inp.value='';
  if(!f)return;
  try{
    const buf=await f.arrayBuffer();
    let d=await ws_api('/api/ws/import',{method:'POST',headers:{'Content-Type':'application/octet-stream'},body:buf});
    ws_log('> Imported '+d.imported+' .lua file(s) from '+f.name+(d.skipped?' ('+d.skipped+' skipped)':''));
    await ws_reset_after_replace();
  }catch(e){ws_log('Import failed: '+e.message);}
}

async function ws_export(){
  if(!(await ws_save_all()))return;
  const a=document.createElement('a');
  a.href='/api/ws/export';
  a.download='workspace.zip';
  document.body.appendChild(a);
  a.click();
  a.remove();
}

async function ws_save_device(){
  if(!(await ws_save_all()))return;
  const r=await ws_dialog({title:'Save On Device',message:'The workspace is saved as a ZIP in the app storage on this device. Name:',input:ws.zip_name,ok:'Save'});
  if(!r.ok||!r.value.trim())return;
  try{
    let name=r.value.trim();
    let list=await ws_api('/api/ws/zips');
    const full=/\.zip$/i.test(name)?name:name+'.zip';
    if((list.zips||[]).some(z=>z.name===full)){
      const o=await ws_dialog({title:'Overwrite?',message:full+' already exists on this device. Replace it?',ok:'Replace',danger:true});
      if(!o.ok)return;
    }
    let d=await ws_post('/api/ws/save_zip',{name:name});
    ws.zip_name=d.name.replace(/\.zip$/i,'');
    ws_log('> Saved on device: '+d.name);
    await ws_refresh_zips();
  }catch(e){ws_log('Save failed: '+e.message);}
}

function ws_format_size(n){
  if(n<1024)return n+' B';
  if(n<1048576)return (n/1024).toFixed(1)+' KB';
  return (n/1048576).toFixed(1)+' MB';
}

async function ws_refresh_zips(){
  const el=document.getElementById('ws-zip-list');
  try{
    if(!ws.info)ws.info=await ws_api('/api/ws/info');
    let d=await ws_api('/api/ws/zips');
    let html='';
    for(const z of d.zips||[]){
      const when=new Date(z.mtime*1000).toLocaleString();
      html+=`<div class="ws-zip-row" data-name="${ws_esc(z.name)}"><div class="ws-zip-name">${ws_esc(z.name)}</div>`+
        `<div class="ws-zip-meta">${ws_format_size(z.size)} - ${ws_esc(when)}</div>`+
        `<div class="ws-zip-btns">`+
        `<button class="btn btn-grn" onclick="ws_load_device_zip(this.closest('.ws-zip-row').dataset.name)"><svg class=i><use href=#i-folder-open></use></svg> Load</button>`+
        `<button class="btn" onclick="ws_download_device_zip(this.closest('.ws-zip-row').dataset.name)"><svg class=i><use href=#i-download></use></svg></button>`+
        `<button class="btn btn-red" onclick="ws_delete_device_zip(this.closest('.ws-zip-row').dataset.name)"><svg class=i><use href=#i-trash-2></use></svg></button>`+
        `</div></div>`;
    }
    el.innerHTML=html||'<div class="ws-hint">No ZIP files saved on this device yet. Use Save On Device to create one.</div>';
    document.getElementById('ws-zip-path').textContent='Folder: '+ws.info.zips+(ws.info.zips_global?'  (shared by every game on this device, ZIP files copied here appear in the list)':'  (per-game fallback, storage access for /sdcard/BNMExplorer/zips was not available)');
  }catch(e){el.innerHTML='<div class="ws-hint">'+ws_esc(e.message)+'</div>';}
}

async function ws_load_device_zip(name){
  const r=await ws_dialog({title:'Load '+name,message:ws_replace_warning,ok:'OK, load',danger:true});
  if(!r.ok)return;
  try{
    let d=await ws_post('/api/ws/load_zip',{name:name});
    ws_log('> Loaded '+d.imported+' .lua file(s) from '+name+(d.skipped?' ('+d.skipped+' skipped)':''));
    await ws_reset_after_replace();
    ws_side_tab('files');
  }catch(e){ws_log('Load failed: '+e.message);}
}

function ws_download_device_zip(name){
  const a=document.createElement('a');
  a.href='/api/ws/zip?name='+encodeURIComponent(name);
  a.download=name;
  document.body.appendChild(a);
  a.click();
  a.remove();
}

async function ws_delete_device_zip(name){
  const r=await ws_dialog({title:'Delete ZIP',message:'Delete '+name+' from this device?',ok:'Delete',danger:true});
  if(!r.ok)return;
  try{
    await ws_post('/api/ws/delete_zip',{name:name});
    await ws_refresh_zips();
  }catch(e){ws_log('Delete failed: '+e.message);}
}

(function(){
  const ed=document.getElementById('lua-editor');
  ed.addEventListener('input',()=>{
    if(!ws.active||!ws.buf[ws.active])return;
    const b=ws.buf[ws.active];
    const was=b.dirty;
    b.text=ed.value;
    b.dirty=true;
    if(!was)ws_render_tabs();
    ws_update_gutter();
  });
  ed.addEventListener('scroll',()=>{document.getElementById('ws-gutter').scrollTop=ed.scrollTop;});
  ed.addEventListener('keydown',e=>{
    if(e.key==='Tab'){
      e.preventDefault();
      const s=ed.selectionStart,t=ed.selectionEnd;
      ed.value=ed.value.substring(0,s)+'  '+ed.value.substring(t);
      ed.selectionStart=ed.selectionEnd=s+2;
      ed.dispatchEvent(new Event('input'));
    }
    if((e.ctrlKey||e.metaKey)&&e.key.toLowerCase()==='s'){
      e.preventDefault();
      if(ws.active)ws_save_file(ws.active);
    }
    if(e.ctrlKey&&e.key==='Enter'){
      e.preventDefault();
      ws_run();
    }
  });
})();

let ftrack_items=[];
let ftrack_timer=null;

function ftrack_on_show(){
  ftrack_poll();
  if(!ftrack_timer)ftrack_timer=setInterval(ftrack_tick,1000);
}

async function ftrack_tick(){
  if(!document.getElementById('pnl-ftrack').classList.contains('active')){clearInterval(ftrack_timer);ftrack_timer=null;return;}
  await ftrack_poll();
}

async function ftrack_poll(){
  let d=await fetch('/api/freeze/tracker').then(r=>r.json()).catch(()=>null);
  if(!d)return;
  ftrack_items=(d.items||[]).slice().sort((a,b)=>b.count-a.count);
  let total=ftrack_items.reduce((s,x)=>s+x.count,0);
  document.getElementById('ftrack-status').textContent=`${d.frozen?'Freeze ON':'Freeze OFF'} | ${ftrack_items.length} unique callers | ${total} blocked`;
  ftrack_render();
}

function ftrack_render(){
  let q=(document.getElementById('ftrack-filter').value||'').toLowerCase();
  let list=q?ftrack_items.filter(x=>(x.caller+' '+x.action+' '+(x.target||'')).toLowerCase().includes(q)):ftrack_items;
  let out=document.getElementById('ftrack-list');
  if(!list.length){
    out.innerHTML='<div style="padding:30px;text-align:center;color:var(--fg-dim);">No blocked attempts recorded yet</div>';
    return;
  }
  out.innerHTML=list.map(x=>{
    let badge=x.in_scene?'<span style="color:#4ec9b0;font-size:11px;margin-left:6px;">[SCENE]</span>':'<span style="color:var(--fg-dim);font-size:11px;margin-left:6px;">[OFF-SCENE]</span>';
    let tgt=x.target&&x.target!=='none'?`<div style="color:var(--fg-dim);font-size:11px;margin-top:2px;">Target: ${esc(x.target)}</div>`:'';
    return `<div class="calls-row" style="align-items:flex-start;"><span class="cr-t" style="color:var(--str);font-weight:700;">x${x.count}</span><div style="flex:1;"><span class="cr-c">${esc(x.caller)}</span> <span style="color:var(--fg-dim)">-&gt;</span> <span class="cr-f">${esc(x.action)}</span>${badge}${tgt}</div></div>`;
  }).join('');
}

async function ftrack_clear(){
  await fetch('/api/freeze/tracker/clear',{method:'POST'});
  ftrack_items=[];
  ftrack_render();
  ftrack_poll();
}

function ic(n){return '<svg class="i"><use href="#i-'+n+'"></use></svg>';}

let mgr_timer=null;
function mgr_fmt_held(s){
  if(s===undefined||s===null)return'-';
  return(s<90?Math.round(s)+' s':Math.floor(s/60)+' m '+Math.round(s%60)+' s');
}
async function mgr_on_show(){
  await mgr_refresh();
  if(mgr_timer)clearInterval(mgr_timer);
  mgr_timer=setInterval(mgr_refresh,2000);
}
async function mgr_refresh(){
  let d=await fetch('/api/manager').then(r=>r.json()).catch(()=>null);
  if(!d)return;
  const holding=d.holding,armed=d.hold;
  const badge=document.getElementById('mgr-state');
  badge.textContent=holding?('FROZEN '+mgr_fmt_held(d.holding_for)):(armed?'Armed for next launch':'Idle');
  badge.style.color=holding?'var(--mth)':(armed?'var(--str)':'var(--fg-dim)');
  document.getElementById('mgr-resume').style.display=holding?'inline-flex':'none';
  document.getElementById('mgr-hold').checked=armed;
  document.getElementById('mgr-timeout').value=d.hold_timeout||0;
  document.getElementById('mgr-autolua').checked=d.auto_lua;
  document.getElementById('mgr-autopath').value=d.auto_lua_path||'main.lua';
  document.getElementById('mgr-boots').textContent=d.boot_count;
  document.getElementById('mgr-resumed').textContent=d.resumed;
  document.getElementById('mgr-lasthold').textContent=d.last_hold_seconds>0?mgr_fmt_held(d.last_hold_seconds):'-';
  document.getElementById('mgr-config').textContent=d.config||'-';
  document.getElementById('mgr-output').textContent=(d.last_output&&d.last_output.length)?d.last_output:'-';
  const b=document.getElementById('boot-banner');
  if(holding){b.style.display='flex';document.getElementById('boot-banner-t').textContent=mgr_fmt_held(d.holding_for);}
  else b.style.display='none';
}
async function mgr_save(){
  let body={
    hold:document.getElementById('mgr-hold').checked,
    auto_lua:document.getElementById('mgr-autolua').checked,
    auto_lua_path:document.getElementById('mgr-autopath').value.trim()||'main.lua',
    hold_timeout:parseInt(document.getElementById('mgr-timeout').value)||0
  };
  let d=await fetch('/api/manager',{method:'POST',headers:{'Content-Type':'application/json'},body:JSON.stringify(body)}).then(r=>r.json()).catch(()=>null);
  if(d)await mgr_refresh();
}
async function mgr_resume(){
  await fetch('/api/manager/resume',{method:'POST'}).catch(()=>{});
  await mgr_refresh();
}
async function mgr_run_auto(){
  let d=await fetch('/api/manager/run_auto_lua',{method:'POST'}).then(r=>r.json()).catch(()=>null);
  if(d&&d.output)document.getElementById('mgr-output').textContent=d.output;
  else if(d&&d.error)document.getElementById('mgr-output').textContent='Error: '+d.error;
  mgr_refresh();
}
setInterval(()=>{fetch('/api/manager').then(r=>r.json()).then(d=>{
  const b=document.getElementById('boot-banner');
  if(d&&d.holding){b.style.display='flex';document.getElementById('boot-banner-t').textContent=mgr_fmt_held(d.holding_for);}
  else if(d&&!d.holding)b.style.display='none';
}).catch(()=>{});},2000);
const cls_icon={class:'box',struct:'square',enum:'list',interface:'circle-dot'};

function side_tab(id){
  document.querySelectorAll('.side-tab').forEach(t=>t.classList.toggle('active',t.dataset.t===id));
  document.querySelectorAll('.side-pane').forEach(p=>p.classList.toggle('active',p.id==='side-'+id));
  if(id==='search')document.getElementById('msearch-q').focus();
}

function asm_node(a,open){
  return `<ul class="tree-node"><div class="tree-nc" onclick="toggle_asm(this,'${esc(a)}')"><span class="arr${open?' open':''}">${ic('chevron-right')}</span><div class="ico">${ic('package')}</div><span class="nm">${esc(a)}</span></div><div class="kids${open?' open':''}"></div></ul>`;
}

function cls_node(asm,c){
  let t=c.t||'class';
  let badge=t!=='class'?`<span class="mono-tag">${esc(t)}</span>`:'';
  return `<div class="tree-nc" onclick="load_cls(this,'${esc(asm)}','${esc(c.ns||'')}','${esc(c.name)}')"><span class="arr"></span><div class="ico t-${t==='interface'?'iface':t}">${ic(cls_icon[t]||'box')}</div><span class="nm">${esc(c.name)}</span>${badge}</div>`;
}

function ns_node(ns,inner,open,count){
  return `<ul class="tree-node"><div class="tree-nc" onclick="toggle_ns(this)"><span class="arr${open?' open':''}">${ic('chevron-right')}</span><div class="ico">${ic('folder')}</div><span class="nm">${esc(ns||'(global)')}</span><span class="cnt">${count}</span></div><div class="kids${open?' open':''}">${inner}</div></ul>`;
}

function group_by_ns(list){
  let by_ns={};
  for(let c of list){let ns=c.ns||'';(by_ns[ns]=by_ns[ns]||[]).push(c);}
  return by_ns;
}

function render_ns_groups(asm,list,open){
  let by_ns=group_by_ns(list),h='';
  for(let ns of Object.keys(by_ns).sort()){
    let items=by_ns[ns].sort((a,b)=>a.name.localeCompare(b.name));
    h+=ns_node(ns,items.map(c=>cls_node(asm,c)).join(''),open,items.length);
  }
  return h;
}

async function init(){
  try{
    let asms=await fetch('/api/assemblies').then(r=>r.json());
    window.asm_list=asms;
    document.getElementById('tree-root').innerHTML=asms.map(a=>asm_node(a,false)).join('');
  }catch(e){}
  load_scene();
}

async function toggle_asm(el,asm){
  let kids=el.nextElementSibling,arr=el.querySelector('.arr');
  if(kids.classList.contains('open')){kids.classList.remove('open');arr.classList.remove('open');return;}
  if(!kids.innerHTML.trim()){
    let classes=await fetch(`/api/classes?a=${encodeURIComponent(asm)}`).then(r=>r.json());
    kids.innerHTML=render_ns_groups(asm,classes,false);
  }
  kids.classList.add('open');arr.classList.add('open');
}

function path_match(a,ns,name,p){
  let has=(s,q)=>!q||String(s||'').toLowerCase().includes(q);
  if(p.length===0)return true;
  if(p.length===1)return has(a,p[0])||has(ns,p[0])||has(name,p[0]);
  if(p.length===2)return has(a,p[0])&&(has(ns,p[1])||has(name,p[1]));
  return has(a,p[0])&&has(ns,p[1])&&has(name,p[2]);
}

let tree_timer=null;
function tree_filter_input(){clearTimeout(tree_timer);tree_timer=setTimeout(tree_filter_apply,140);}

async function tree_filter_apply(){
  let q=document.getElementById('tree-q').value.trim();
  let root=document.getElementById('tree-root');
  if(!q){root.innerHTML=(window.asm_list||[]).map(a=>asm_node(a,false)).join('');return;}
  if(!all_cls_cache)all_cls_cache=await fetch('/api/allclasses').then(r=>r.json());
  let parts=q.toLowerCase().split('/').map(s=>s.trim());
  while(parts.length&&!parts[parts.length-1])parts.pop();
  let res=[];
  for(let c of all_cls_cache){if(path_match(c.a,c.ns,c.name,parts)){res.push(c);if(res.length>=3000)break;}}
  let by_asm={};
  for(let c of res)(by_asm[c.a]=by_asm[c.a]||[]).push(c);
  let h='',many=res.length>300;
  for(let a of Object.keys(by_asm).sort()){
    h+=`<ul class="tree-node"><div class="tree-nc" onclick="toggle_ns(this)"><span class="arr open">${ic('chevron-right')}</span><div class="ico">${ic('package')}</div><span class="nm">${esc(a)}</span><span class="cnt">${by_asm[a].length}</span></div><div class="kids open">${render_ns_groups(a,by_asm[a],!many)}</div></ul>`;
  }
  root.innerHTML=h||'<div class="ws-hint">No classes match</div>';
}

function toggle_ns(el){el.nextElementSibling.classList.toggle('open');el.querySelector('.arr').classList.toggle('open');}

const ms={kinds:{class:true,method:true,field:true,property:true},hits:[],seq:0,timer:null};

function ms_chip(k){
  ms.kinds[k]=!ms.kinds[k];
  document.getElementById('chip-'+k).classList.toggle('on',ms.kinds[k]);
  ms_input();
}

function ms_input(){clearTimeout(ms.timer);ms.timer=setTimeout(ms_run,220);}

async function ms_run(){
  let q=document.getElementById('msearch-q').value.trim();
  let path=document.getElementById('msearch-path').value.trim();
  let st=document.getElementById('msearch-status'),list=document.getElementById('msearch-list');
  if(!q&&!path){st.textContent='';list.innerHTML='';ms.hits=[];return;}
  let kinds=Object.keys(ms.kinds).filter(k=>ms.kinds[k]).join(',');
  if(!kinds){st.textContent='Pick at least one kind';return;}
  let my=++ms.seq;
  st.textContent='Searching...';
  let d=await fetch(`/api/search?q=${encodeURIComponent(q)}&path=${encodeURIComponent(path)}&kinds=${kinds}&limit=500`).then(r=>r.json()).catch(()=>null);
  if(my!==ms.seq)return;
  if(!d){st.textContent='Search failed';return;}
  ms.hits=d.r;
  st.textContent=`${d.r.length}${d.truncated?'+':''} results in ${d.scanned} classes`;
  list.innerHTML=d.r.map((h,i)=>{
    let sig=h.k==='method'?`(${esc(h.p||'')}) : ${esc(h.t)}`:(h.k==='class'?esc(h.t):': '+esc(h.t));
    let full=(h.ns?h.ns+'.':'')+h.c;
    return `<div class="sr-row" onclick="ms_open(${i})" title="${esc(h.a+'/'+full+(h.k==='class'?'':'.'+h.n))}"><span class="sr-k ${h.k}"></span><span class="sr-n">${esc(h.n)}</span><span class="sr-t">${sig}</span><span class="sr-c">${h.k==='class'?esc(h.ns||h.a):esc(h.c)}</span></div>`;
  }).join('');
}

function ms_open(i){
  let h=ms.hits[i];if(!h)return;
  toggle_sidebar(false);
  if(h.k==='class'){load_cls(null,h.a,h.ns||'',h.c);return;}
  open_member(h.a,h.ns||'',h.c,h.n);
}

function open_member(asm,ns,cls,member){
  cur_asm=asm;cur_ns=ns;cur_cls=cls;
  document.getElementById('ctrl-addr').value='0x0';
  document.getElementById('ctrl-asm').value=asm;
  document.getElementById('ctrl-ns').value=ns;
  document.getElementById('ctrl-cls').value=cls;
  document.getElementById('ctrl-static').checked=true;
  document.getElementById('ctrl-addr').disabled=true;
  insp.filter=member||'';
  switch_tab('ctrl');
  load_controller();
}

function code_hl_type(t){return `<span class="c-ty">${esc(t)}</span>`;}

function format_code(d){
  let ns=d.ns||'',name=d.name||'',parent=d.parent||'';
  let t=d.type||'class';
  let ind=ns?'    ':'';
  let lines=[];
  if(ns)lines.push(`<span class="c-kw">namespace</span> <span class="c-ty">${esc(ns)}</span> {`);
  let decl=`${ind}<span class="c-kw">public ${t}</span> <span class="c-ty">${esc(name)}</span>`;
  if(parent&&parent!=='Object'&&parent!=='ValueType')decl+=` <span class="c-op">:</span> <span class="c-ty">${esc(parent)}</span>`;
  lines.push(decl+' {');
  if((d.fields||[]).length)lines.push(`${ind}    <span class="c-cm">// fields</span>`);
  for(let f of d.fields||[]){
    let st=f.s?' <span class="c-kw">static</span>':'';
    lines.push(`${ind}    <span class="c-kw">public</span>${st} ${code_hl_type(f.type)} <span>${esc(f.name)}</span>; <span class="c-cm">// 0x${f.off?Number(f.off).toString(16):'?'}</span>`);
  }
  if((d.props||[]).length)lines.push('',`${ind}    <span class="c-cm">// properties</span>`);
  for(let p of d.props||[]){
    let acc=(p.g?'get; ':'')+(p.s?'set; ':'');
    lines.push(`${ind}    <span class="c-kw">public</span> ${code_hl_type(p.type)} <span>${esc(p.name)}</span> { <span class="c-kw">${acc}</span>}`);
  }
  if((d.methods||[]).length)lines.push('',`${ind}    <span class="c-cm">// methods</span>`);
  for(let m of d.methods||[]){
    let prms=(m.params||[]).map(p=>`${code_hl_type(p.t)} ${esc(p.n)}`).join(', ');
    let st=m.s?' <span class="c-kw">static</span>':'';
    let addr=m.addr?` <span class="c-cm">// ${esc(m.addr)}</span>`:'';
    lines.push(`${ind}    <span class="c-kw">public</span>${st} ${code_hl_type(m.ret)} <span class="c-mt">${esc(m.name)}</span>(${prms}) {}${addr}`);
  }
  lines.push(ind+'}');
  if(ns)lines.push('}');
  return lines.join('\n');
}

async function load_cls(el,asm,ns,name){
  document.querySelectorAll('#tree-root .tree-nc').forEach(e=>e.classList.remove('active'));
  if(el)el.classList.add('active');
  cur_asm=asm;cur_ns=ns;cur_cls=name;
  toggle_sidebar(false);
  let d=await fetch(`/api/class?a=${encodeURIComponent(asm)}&ns=${encodeURIComponent(ns)}&n=${encodeURIComponent(name)}`).then(r=>r.json());
  let bar=`<div class="mem-bar" style="white-space:normal;position:sticky;top:-8px;margin:-8px -12px 8px;">
    <span class="mem-title grow">${esc((ns?ns+'.':'')+name)}</span>
    <button class="btn btn-sm" onclick="switch_tab('inst')">${ic('database')} Instances</button>
    <button class="btn btn-sm btn-blu" onclick="open_static_controller()">${ic('gamepad-2')} Inspect static</button>
    <button class="btn btn-sm" onclick="calls_watch_class('${esc(ns)}','${esc(name)}')">${ic('eye')} Watch all</button>
  </div>`;
  document.getElementById('pnl-code').innerHTML=bar+format_code(d);
  switch_tab('code');
  load_instances();
}

const insp={d:null,asm:'',ns:'',cls:'',addr:'0x0',scope:'all',kinds:{property:true,field:true,method:true},filter:'',auto:false,timer:null,watched:{}};

function insp_short(t){return String(t||'').replace(/^System\./,'');}

async function insp_load_watched(){
  insp.watched={};
  let d=await fetch('/api/calls/methods').then(r=>r.json()).catch(()=>null);
  for(let m of (d&&d.methods)||[])if(m.a)insp.watched[m.a.toUpperCase()]=m.id;
}

async function load_controller(keep){
  let addr=document.getElementById('ctrl-addr').value;
  let asm=document.getElementById('ctrl-asm').value;
  let ns=document.getElementById('ctrl-ns').value;
  let cls=document.getElementById('ctrl-cls').value;
  let is_static=document.getElementById('ctrl-static').checked;
  let box=document.getElementById('ctrl-insp');
  if(!keep)box.innerHTML='<div class="ws-hint" style="text-align:center;padding:30px;">Loading target...</div>';
  let d=await fetch(`/api/controller/inspect?addr=${addr}&asm=${encodeURIComponent(asm)}&ns=${encodeURIComponent(ns)}&cls=${encodeURIComponent(cls)}`).then(r=>r.json()).catch(()=>({}));
  if(!d.name){box.innerHTML='<div class="ws-hint" style="text-align:center;padding:30px;color:var(--mth);">Target class or instance not found</div>';return;}
  await insp_load_watched();
  insp.d=d;insp.addr=addr;insp.asm=asm;insp.ns=ns;insp.cls=cls;
  if(!keep)insp.scope='all';
  if(keep&&document.getElementById('insp-list'))insp_list();
  else insp_render();
}

function insp_render(){
  let d=insp.d,box=document.getElementById('ctrl-insp');
  let full=(d.ns?d.ns+'.':'')+d.name;
  let inst=/^0x0*$/.test(insp.addr)||document.getElementById('ctrl-static').checked?'static (no instance)':'instance '+esc(insp.addr);
  let seg=(id,l)=>`<button class="${insp.scope===id?'on':''}" onclick="insp_scope('${id}')">${l}</button>`;
  let chip=(k,l)=>`<span class="chip k-${k}${insp.kinds[k]?' on':''}" onclick="insp_kind('${k}')"><span class="sq"></span>${l}</span>`;
  box.innerHTML=`<div class="mem-head"><div class="mem-title">${esc(full)}</div><div class="mem-sub">Assembly: ${esc(d.asm||insp.asm)} &middot; ${inst}</div></div>
  <div class="mem-bar"><span class="mem-name"><span class="lab">Filter</span></span>
    <input class="inp grow" id="insp-filter" placeholder="Filter names..." value="${esc(insp.filter)}" oninput="insp.filter=this.value;insp_list()">
    <button class="btn btn-sm" onclick="load_controller(true)">${ic('refresh-cw')} Update values</button>
    <label class="chip${insp.auto?' on':''}" onclick="insp_auto(this)"><span class="sq"></span>Auto-update</label></div>
  <div class="mem-bar"><span class="lab" style="color:var(--fg-mut)">Scope</span><div class="seg">${seg('all','All')}${seg('instance','Instance')}${seg('static','Static')}</div>
    <div class="chips">${chip('property','Property')}${chip('field','Field')}${chip('method','Method')}</div></div>
  <div id="insp-list"></div>`;
  insp_list();
}

function insp_scope(s){insp.scope=s;insp_render();}
function insp_kind(k){insp.kinds[k]=!insp.kinds[k];insp_render();}
function insp_auto(el){
  insp.auto=!insp.auto;el.classList.toggle('on',insp.auto);
  clearInterval(insp.timer);insp.timer=null;
  if(insp.auto)insp.timer=setInterval(()=>{
    if(!document.getElementById('pnl-ctrl').classList.contains('active')){return;}
    let a=document.activeElement;
    if(a&&a.closest&&a.closest('#insp-list'))return;
    load_controller(true);
  },1500);
}

function value_inner(r,da,dis){
  let html=build_field_row(r,da,dis,true,true);
  let s=html.indexOf('<div class="val">')+17;
  return html.slice(s,html.length-12);
}

function insp_list(){
  let d=insp.d;if(!d)return;
  let q=(insp.filter||'').toLowerCase(),rows=[];
  let ok_scope=s=>insp.scope==='all'||(insp.scope==='static'?s:!s);
  for(let f of d.fields||[]){
    let kind=f.isProp?'property':'field';
    if(!insp.kinds[kind]||!ok_scope(!!f.static))continue;
    if(q&&!(f.name||'').toLowerCase().includes(q))continue;
    rows.push({kind,f});
  }
  rows.sort((a,b)=>(a.kind===b.kind?0:(a.kind==='property'?-1:1)));
  if(insp.kinds.method){
    (d.methods||[]).forEach((m,idx)=>{
      if(!ok_scope(!!m.s))return;
      if(q&&!(m.name||'').toLowerCase().includes(q))return;
      rows.push({kind:'method',m,idx});
    });
  }
  let total=rows.length;
  rows=rows.slice(0,700);
  let h='';
  for(let r of rows){
    if(r.kind==='method')h+=method_row(r.m,r.idx);
    else h+=field_row(r.f,r.kind);
  }
  if(total>rows.length)h+=`<div class="ws-hint">Showing ${rows.length} of ${total}, refine the filter</div>`;
  document.getElementById('insp-list').innerHTML=h||'<div class="ws-hint" style="padding:20px;text-align:center;">Nothing matches</div>';
}

function field_row(f,kind){
  let da=`data-addr="${esc(insp.addr)}" data-tt="instance" data-pid="${esc(f.name)}" data-vt="${esc(f.type)}" data-ip="${f.isProp?'true':'false'}" data-static="${f.static?'true':'false'}"`;
  let dis=f.canWrite===false?'disabled':'';
  return `<div class="mrow"><div class="mn k-${kind}"><span class="d">${esc(f.decl||insp.cls)}.</span>${esc(f.name)}${f.static?'<span class="sc">static</span>':''}</div><div class="mv"><div class="mline"><span class="ty">${esc(insp_short(f.type))}</span>${value_inner(f,da,dis)}</div></div></div>`;
}

function method_row(m,idx){
  let prim=['System.Int32','Int32','int','System.Single','Single','float','System.Double','Double','double','System.Boolean','Boolean','bool','System.String','String','string','System.Int64','Int64','long','System.UInt32','UInt32','uint','System.Byte','Byte','byte','System.Int16','Int16','short','UnityEngine.Vector3','Vector3','UnityEngine.Color','Color','UnityEngine.Vector2','Vector2'];
  let params='';
  (m.params||[]).forEach((p,pidx)=>{
    let is_obj=!prim.includes(p.t);
    let input_id=`marg-input-${idx}-${pidx}`;
    params+=`<div class="mline"><span class="ty">${esc(insp_short(p.t))}</span><input type="text" id="${input_id}" class="inp marg-${idx}" data-type="${esc(p.t)}" placeholder="${is_obj?'0x... '+esc(p.n):esc(p.n)}">`;
    if(is_obj)params+=`<button class="btn btn-sm btn-blu" onclick="open_instance_picker('${esc(p.t)}','${input_id}')" title="Pick live instance">${ic('mouse-pointer-click')}</button>`;
    params+='</div>';
  });
  let watched=m.addr&&insp.watched[String(m.addr).toUpperCase()]!==undefined;
  let n=(m.params||[]).length;
  return `<div class="mrow"><div class="mn k-method"><span class="d">${esc(m.decl||insp.cls)}.</span>${esc(m.name)}${m.s?'<span class="sc">static</span>':''}</div><div class="mv">${params}<div class="mline"><button class="btn btn-sm" onclick="exec_method('${esc(insp.asm)}','${esc(insp.ns)}','${esc(insp.cls)}','${esc(m.name)}',${m.s?'true':'false'},'${esc(insp.addr)}',${idx})">Evaluate (${n})</button><button class="btn btn-sm${watched?' btn-grn':''}" title="Watch calls and callers" onclick="watch_member('${esc(m.addr||'')}',this)">${ic(watched?'eye-off':'eye')}</button><span class="ev" id="mres-${idx}">Not yet evaluated (${esc(insp_short(m.ret))})</span></div></div></div>`;
}

async function watch_member(addr,btn){
  if(!addr)return;
  let key=addr.toUpperCase();
  if(insp.watched[key]!==undefined){
    await fetch('/api/calls/unwatch_method',{method:'POST',headers:{'Content-Type':'application/json'},body:JSON.stringify({m:insp.watched[key]})});
    delete insp.watched[key];
    btn.classList.remove('btn-grn');btn.innerHTML=ic('eye');return;
  }
  let d=await fetch('/api/calls/watch_method',{method:'POST',headers:{'Content-Type':'application/json'},body:JSON.stringify({asm:insp.asm,ns:insp.ns,cls:insp.cls,addr})}).then(r=>r.json()).catch(()=>({error:'request failed'}));
  if(d.ok){insp.watched[key]=d.m;btn.classList.add('btn-grn');btn.innerHTML=ic('eye-off');}
  else alert('Watch failed: '+(d.error||'unknown'));
}

async function exec_method(asm,ns,cls,m_name,is_static,addr,idx){
  let args=Array.from(document.querySelectorAll(`.marg-${idx}`)).map(el=>({t:el.getAttribute('data-type'),v:el.value}));
  let res=document.getElementById(`mres-${idx}`);
  res.className='ev';res.textContent='Invoking...';
  let d=await fetch('/api/invoke',{method:'POST',headers:{'Content-Type':'application/json'},body:JSON.stringify({asm,ns,cls,method:m_name,static:is_static,instance:addr,args})}).then(r=>r.json()).catch(()=>({ok:false,error:'request failed'}));
  let rm=d.ok?String(d.value).match(/ @ 0x([0-9A-Fa-f]+)/):null;
  let open_btn=rm&&!/^0+$/.test(rm[1])?` <button class="btn btn-sm" onclick="open_object_ref('${rm[1]}')">${ic('external-link')}</button>`:'';
  res.className='ev ok';
  res.innerHTML=d.ok?`${esc(d.value)}${open_btn}`:`<span style="color:var(--mth)">${esc(d.error)}</span>`;
}

function switch_tab(id){
  document.querySelectorAll('.tab').forEach(t=>t.classList.remove('active'));
  document.querySelectorAll('.panel').forEach(p=>p.classList.remove('active'));
  document.getElementById(`tab-${id}`).classList.add('active');
  document.getElementById(`pnl-${id}`).classList.add('active');
  if(id==='loops')refresh_loops_ui();
  if(id==='calls')calls_on_show();
  if(id==='lua')ws_on_show();
  if(id==='ftrack')ftrack_on_show();
  if(id==='mgr')mgr_on_show();
}

function do_search(e){
  if(e.key!=='Enter')return;
  let q=document.getElementById('gSearch').value.trim();
  document.getElementById('msearch-q').value=q;
  document.getElementById('msearch-path').value='';
  side_tab('search');
  toggle_sidebar(true);
  ms_run();
}

function calls_render(){
  let q=(document.getElementById('calls-filter').value||'').toLowerCase();
  let items=calls_items;
  if(q)items=items.filter(i=>((i.c||'')+'::'+(i.f||'')+' '+(i.a||'')+' '+((i.cb||[]).join(' '))).toLowerCase().includes(q));
  let t0=items.length?items[0].t:0;
  let out=document.getElementById('calls-log');
  let h=items.slice(-1500).map(i=>{
    let inst=i.i?` <span style="color:var(--fg-dim)">${esc(i.i)}</span>`:'';
    let cb=(i.cb&&i.cb.length)?`<span class="cr-cb" title="${esc(i.cb.join('\n'))}" onclick="calls_pick_caller(this.dataset.c)" data-c="${esc(i.cb[0])}">&larr; ${esc(i.cb[0])}</span>`:'';
    return `<div class="calls-row"><span class="cr-t">${((i.t-t0)/1000).toFixed(2)}</span><span><span class="cr-c" onclick="calls_show_callers(${i.m})" title="Show callers">${esc(i.c||'?')}</span>::<span class="cr-f" onclick="calls_ignore_one(${i.m},true)" title="Tap to ignore this method">${esc(i.f||'?')}</span>${inst}(<span class="cr-a">${esc(i.a)}</span>)</span>${cb}</div>`;
  }).join('');
  out.innerHTML=h;
  if(document.getElementById('calls-follow').checked)out.scrollTop=out.scrollHeight;
}

function calls_pick_caller(c){
  document.getElementById('calls-filter').value=c;
  calls_render();
}

async function calls_show_callers(id){
  let d=await fetch('/api/calls/callers?m='+id).then(r=>r.json()).catch(()=>null);
  let box=document.getElementById('calls-callers');
  if(!d||!d.callers||!d.callers.length){box.style.display='none';return;}
  box.style.display='flex';
  box.innerHTML='<span style="color:var(--fg-mut)">Callers:</span>'+d.callers.map(c=>`<span data-c="${esc(c.c)}" onclick="calls_pick_caller(this.dataset.c)">${esc(c.c)}<b>${c.n}</b></span>`).join('')+`<span style="margin-left:auto" onclick="this.parentElement.style.display='none'">${ic('x')}</span>`;
}

async function calls_watch_class(ns,cls){
  let r=await fetch('/api/calls/watch',{method:'POST',headers:{'Content-Type':'application/json'},body:JSON.stringify({asm:'',ns,cls})}).then(r=>r.json()).catch(()=>({error:'request failed'}));
  if(r.error){alert(r.error);return;}
  switch_tab('calls');
}

const lua_keywords=new Set(['and','break','do','else','elseif','end','false','for','function','goto','if','in','local','nil','not','or','repeat','return','then','true','until','while']);
const lua_api_list=[
  ['find_class','(name [, assembly])','class'],['find_object','(class)','object'],['find_objects_of_type','(class)','list'],
  ['hook_method','(class, method, count|"sig", fn [, asm])','hook'],['unhook_method','(id)','hook'],['unhook_all_methods','()','hook'],
  ['hook_list','()','hook'],['hook_original','(...)','hook'],['hook_caller','()','hook'],['has_method_hook','(id)','hook'],['HOOK_NULL','','hook'],
  ['get_hook','(id)','hook'],['set_hook_callback','(id, fn)','hook'],
  ['find_classes','(text [, path, limit])','reflect'],['find_methods','(text [, path, limit])','reflect'],['find_fields','(text [, path, limit])','reflect'],
  ['find_properties','(text [, path, limit])','reflect'],['find_members','(text [, path, limit])','reflect'],['list_assemblies','()','reflect'],
  ['list_classes','([path, limit])','reflect'],['dump_class','(class)','reflect'],
  ['print','(...)','core'],['require','(module)','core'],['every','(seconds, fn)','task'],['after','(seconds, fn)','task'],['cancel','(id)','task'],['cancel_all','()','task'],
  ['on_update','(fn)','task'],['set_tick_hook','(fn)','task'],['has_tick_hook','()','task'],['on_game_start','(fn)','task'],['main_thread','(fn)','task'],
  ['get_time','()','core'],['type_name','(value)','core'],
  ['vec2','(x, y)','math'],['vec3','(x, y, z)','math'],['vec4','(x, y, z, w)','math'],['quat','(x, y, z, w)','math'],['color','(r, g, b [, a])','math'],
  ['destroy','(obj)','core'],['is_valid','(obj)','core'],['wrap','(addr, class)','core'],['get_builtin_resource','(type, name)','core'],
  ['http','','net'],['json','','net'],['fs','','files'],['memory','','core'],['bit','','core'],['base64','','core'],['pack','','core'],
  ['http.get_async','(url, fn)','net'],['http.post_async','(url, body, fn)','net'],['http.request_async','(options, fn)','net'],
  ['http.set_ssl_verify','(on)','net'],['http.set_ca_file','(path)','net'],
  ['audio','','audio'],['audio.load','(path_or_bytes)','audio'],['audio.play','(clip, options)','audio'],['audio.stop','(handle)','audio'],
  ['audio.decode','(path_or_bytes)','audio'],['audio.route_to_mic','(clip, loop)','audio'],['audio.restore_mic','()','audio'],
  ['dev','','core'],['dev.guard','(fn)','core'],['dev.self_test','()','core'],['dev.info','()','core'],['dev.main_tasks','(on)','core'],
  ['dev.debug_pcall','(on)','core'],['dev.guard_calls','(on)','core'],['dev.log_path','()','core'],['dev.log_clear','()','core'],
  ['has_class','(name [, assembly])','core'],
  ['enum_values','(class)','core'],['enum_name','(class, value)','core'],
  ['to_table','(array_or_list)','core'],['make_array','(class, table)','core'],['make_byte_array','(string)','core'],
  ['make_float_array','(table_or_string)','core'],['array_set','(arr, table [, offset])','core'],['array_set_floats','(arr, values [, offset])','core'],
  ['array_to_string','(arr)','core'],
  ['hex','(number)','core'],['parse_hex','(string)','core'],['set_timescale','(value)','core'],['get_timescale','()','core'],
  ['log_info','(...)','core'],['log_warn','(...)','core'],['log_error','(...)','core'],['il2cpp_loaded','()','core'],
  ['scene','','core'],['xr','','core']
];
const lua_member_sugar=['list_methods','list_fields','list_properties','list_members','list_parents','list_interfaces','list_nested','class_info','dump','get_method','get_field','set_field','get_component','add_component','get_component_in_children','get_class','debug_info','new','is_valid'];
const lua_api_names=new Set(lua_api_list.map(a=>a[0]));

function lua_hl_esc(s){return s.replace(/&/g,'&amp;').replace(/</g,'&lt;').replace(/>/g,'&gt;');}

function lua_highlight(src){
  let out='',i=0,n=src.length;
  const span=(c,t)=>`<span class="${c}">${lua_hl_esc(t)}</span>`;
  while(i<n){
    let c=src[i];
    if(c==='-'&&src[i+1]==='-'){
      let m=/^--\[(=*)\[/.exec(src.slice(i,i+40));
      let end;
      if(m){let close=']'+m[1]+']';end=src.indexOf(close,i+m[0].length);end=end<0?n:end+close.length;}
      else{end=src.indexOf('\n',i);if(end<0)end=n;}
      out+=span('c-cm',src.slice(i,end));i=end;
    }else if(c==='"'||c==="'"){
      let j=i+1;
      while(j<n&&src[j]!==c&&src[j]!=='\n'){if(src[j]==='\\')j++;j++;}
      j=Math.min(n,j+1);
      out+=span('c-st',src.slice(i,j));i=j;
    }else if(c==='['&&/^\[=*\[/.test(src.slice(i,i+20))){
      let m=/^\[(=*)\[/.exec(src.slice(i,i+20));
      let close=']'+m[1]+']';
      let end=src.indexOf(close,i+m[0].length);end=end<0?n:end+close.length;
      out+=span('c-st',src.slice(i,end));i=end;
    }else if(/[0-9]/.test(c)||(c==='.'&&/[0-9]/.test(src[i+1]||''))){
      let m=/^(0[xX][0-9a-fA-F]+|\d*\.?\d+([eE][+-]?\d+)?|\d+\.)/.exec(src.slice(i,i+40));
      let t=m?m[0]:c;
      out+=span('c-nm',t);i+=t.length;
    }else if(/[A-Za-z_]/.test(c)){
      let j=i+1;
      while(j<n&&/[A-Za-z0-9_]/.test(src[j]))j++;
      let w=src.slice(i,j);
      let k=j;while(k<n&&(src[k]===' '||src[k]==='\t'))k++;
      let prev=i>0?src[i-1]:'';
      if(lua_keywords.has(w))out+=span('c-kw',w);
      else if(lua_api_names.has(w)&&prev!=='.'&&prev!==':')out+=span('c-gl',w);
      else if(src[k]==='('||(prev===':'&&src[k]==='('))out+=span('c-fn',w);
      else out+=lua_hl_esc(w);
      i=j;
    }else if('+-*/%^#&~|<>=.:,;(){}[]'.includes(c)){
      out+=span('c-op',c);i++;
    }else{out+=lua_hl_esc(c);i++;}
  }
  return out+'\n';
}

function lua_hl_sync(){
  let ed=document.getElementById('lua-editor'),hl=document.getElementById('lua-hl');
  hl.innerHTML=lua_highlight(ed.value);
  hl.scrollTop=ed.scrollTop;hl.scrollLeft=ed.scrollLeft;
}

const ac={items:[],sel:0,open:false,start:0,class_cache:{},char_w:7.2};

function ac_measure(){
  let cv=document.createElement('canvas');
  let c=cv.getContext&&cv.getContext('2d');
  if(!c)return;
  c.font="12px 'JetBrains Mono',ui-monospace,Consolas,monospace";
  ac.char_w=c.measureText('M').width||7.2;
}

function ac_find_full(name){
  if(!all_cls_cache)return null;
  let want=name.toLowerCase();
  return all_cls_cache.find(c=>((c.ns?c.ns+'.':'')+c.name).toLowerCase()===want)||all_cls_cache.find(c=>c.name.toLowerCase()===want)||null;
}

async function ac_class_members(full){
  if(ac.class_cache[full])return ac.class_cache[full];
  let c=ac_find_full(full);
  if(!c)return [];
  let d=await fetch(`/api/class?a=${encodeURIComponent(c.a)}&ns=${encodeURIComponent(c.ns||'')}&n=${encodeURIComponent(c.name)}`).then(r=>r.json()).catch(()=>null);
  let items=[];
  if(d){
    for(let f of d.fields||[])items.push({t:f.name,k:'fld',d:insp_short(f.type)});
    for(let p of d.props||[])items.push({t:p.name,k:'prop',d:insp_short(p.type)});
    for(let m of d.methods||[])items.push({t:m.name,k:'fn',d:'('+(m.params||[]).map(p=>insp_short(p.t)+' '+p.n).join(', ')+') '+insp_short(m.ret)});
  }
  ac.class_cache[full]=items;
  return items;
}

function ac_vars(text){
  let vars={},re=/(?:local\s+)?([A-Za-z_][A-Za-z0-9_]*)\s*=\s*find_class\(\s*["']([^"']+)["']/g,m;
  while((m=re.exec(text)))vars[m[1]]=m[2];
  return vars;
}

async function ac_collect(text,pos){
  let before=text.slice(0,pos);
  let line=before.slice(before.lastIndexOf('\n')+1);
  let m;
  if((m=/(?:find_class|find_object|find_objects_of_type|dump_class)\(\s*["']([^"']*)$/.exec(line))){
    if(!all_cls_cache)all_cls_cache=await fetch('/api/allclasses').then(r=>r.json()).catch(()=>[]);
    let q=m[1].toLowerCase(),out=[];
    for(let c of all_cls_cache){
      let full=(c.ns?c.ns+'.':'')+c.name;
      if(full.toLowerCase().includes(q)){out.push({t:full,k:'cls',d:c.a});if(out.length>=60)break;}
    }
    return {start:pos-m[1].length,items:out};
  }
  if((m=/hook_method\(\s*["']([^"']+)["']\s*,\s*["']([^"']*)$/.exec(line))){
    let items=(await ac_class_members(m[1])).filter(i=>i.k==='fn'&&i.t.toLowerCase().includes(m[2].toLowerCase()));
    return {start:pos-m[2].length,items:items.slice(0,60)};
  }
  if((m=/([A-Za-z_][A-Za-z0-9_]*)\s*[.:]([A-Za-z0-9_]*)$/.exec(line))){
    let vars=ac_vars(text),q=m[2].toLowerCase(),items=[];
    if(vars[m[1]]){
      items=(await ac_class_members(vars[m[1]])).filter(i=>i.t.toLowerCase().startsWith(q)||(q.length>1&&i.t.toLowerCase().includes(q)));
    }
    for(let s of lua_member_sugar)if(s.startsWith(q)&&vars[m[1]])items.push({t:s,k:'api',d:'LuaBNM'});
    return {start:pos-m[2].length,items:items.slice(0,60)};
  }
  if((m=/([A-Za-z_][A-Za-z0-9_]*)$/.exec(line))){
    let q=m[1].toLowerCase(),items=[],seen=new Set();
    if(q.length<1)return null;
    for(let [n,sig,k] of lua_api_list)if(n.toLowerCase().startsWith(q)){items.push({t:n,k:'api',d:sig});seen.add(n);}
    for(let k of lua_keywords)if(k.startsWith(q)&&k!==q){items.push({t:k,k:'kw',d:''});seen.add(k);}
    let words=text.match(/[A-Za-z_][A-Za-z0-9_]{2,}/g)||[];
    for(let w of words)if(!seen.has(w)&&w!==m[1]&&w.toLowerCase().startsWith(q)){seen.add(w);items.push({t:w,k:'var',d:''});if(items.length>40)break;}
    return {start:pos-m[1].length,items};
  }
  return null;
}

function ac_caret_xy(ed){
  let before=ed.value.slice(0,ed.selectionStart);
  let lines=before.split('\n');
  let row=lines.length-1,col=lines[row].length;
  return {x:10+col*ac.char_w-ed.scrollLeft,y:10+(row+1)*18-ed.scrollTop};
}

function ac_render(){
  let pop=document.getElementById('lua-ac'),ed=document.getElementById('lua-editor');
  if(!ac.items.length){pop.classList.remove('open');ac.open=false;return;}
  let xy=ac_caret_xy(ed);
  pop.style.left=Math.max(0,Math.min(xy.x,ed.clientWidth-220))+'px';
  pop.style.top=Math.min(xy.y,Math.max(0,ed.clientHeight-200))+'px';
  pop.innerHTML=ac.items.map((it,i)=>`<div class="ac-item${i===ac.sel?' sel':''}" data-i="${i}"><span class="k">${it.k}</span><span>${lua_hl_esc(it.t)}</span><span class="d">${lua_hl_esc(it.d||'')}</span></div>`).join('')+'<div class="ac-foot">Up/Down select, Enter or Tab accept, Esc close</div>';
  pop.classList.add('open');ac.open=true;
  let s=pop.querySelector('.ac-item.sel');if(s)s.scrollIntoView({block:'nearest'});
}

function ac_close(){ac.items=[];document.getElementById('lua-ac').classList.remove('open');ac.open=false;}

let ac_seq=0;
async function ac_update(force){
  let ed=document.getElementById('lua-editor'),my=++ac_seq;
  if(ed.selectionStart!==ed.selectionEnd){ac_close();return;}
  let r=await ac_collect(ed.value,ed.selectionStart).catch(()=>null);
  if(my!==ac_seq)return;
  if(!r||!r.items.length||(!force&&r.items.length===1&&r.items[0].t===ed.value.slice(r.start,ed.selectionStart))){ac_close();return;}
  ac.start=r.start;ac.items=r.items;ac.sel=0;ac_render();
}

function ac_accept(i){
  let it=ac.items[i],ed=document.getElementById('lua-editor');
  if(!it)return;
  let pos=ed.selectionStart;
  ed.value=ed.value.slice(0,ac.start)+it.t+ed.value.slice(pos);
  ed.selectionStart=ed.selectionEnd=ac.start+it.t.length;
  ac_close();
  ed.dispatchEvent(new Event('input'));
}

(function(){
  const ed=document.getElementById('lua-editor');
  ac_measure();
  ed.addEventListener('input',()=>{lua_hl_sync();ac_update(false);});
  ed.addEventListener('scroll',()=>{
    let hl=document.getElementById('lua-hl');hl.scrollTop=ed.scrollTop;hl.scrollLeft=ed.scrollLeft;
    if(ac.open)ac_render();
  });
  ed.addEventListener('blur',()=>setTimeout(ac_close,150));
  ed.addEventListener('keydown',e=>{
    if(ac.open){
      if(e.key==='ArrowDown'||e.key==='ArrowUp'){
        e.preventDefault();e.stopImmediatePropagation();
        ac.sel=(ac.sel+(e.key==='ArrowDown'?1:ac.items.length-1))%ac.items.length;ac_render();return;
      }
      if(e.key==='Enter'||e.key==='Tab'){
        e.preventDefault();e.stopImmediatePropagation();ac_accept(ac.sel);return;
      }
      if(e.key==='Escape'){e.preventDefault();e.stopImmediatePropagation();ac_close();return;}
    }
    if(e.ctrlKey&&e.code==='Space'){e.preventDefault();e.stopImmediatePropagation();ac_update(true);}
  },true);
  document.getElementById('lua-ac').addEventListener('mousedown',e=>{
    let el=e.target.closest('.ac-item');
    if(el){e.preventDefault();ac_accept(parseInt(el.dataset.i));}
  });
})();

init();
</script>
</body>
</html>)HTML";

}

std::string get_explorer_html(int asm_count) {
 std::string html = k_html_a;
 html += std::to_string(asm_count);
 html += k_html_b;
 return html;
}
