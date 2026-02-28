#pragma once
#include <string>

inline std::string GetExplorerHTML(int asmCount) {
    std::string html = R"HTMLEND(<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width,initial-scale=1.0">
<title>Unity - BNM Explorer</title>
<style>
:root {
    --bg-main: #383838;
    --bg-side: #383838;
    --bg-menu: #3e3e42;
    --bg-hover: #4d4d4d;
    --bg-sel: #2c5d87;
    --fg: #cccccc;
    --kw: #569cd6;
    --type: #4ec9b0;
    --mth: #dcdcaa;
    --str: #d69d85;
    --num: #b5cea8;
    --border: #242424;
    --comp-hdr: #3e3e3e;
    --input-bg: #2a2a2a;
    --input-brd: #1a1a1a;
}
* { box-sizing: border-box; margin: 0; padding: 0; font-family: -apple-system, BlinkMacSystemFont, "Segoe UI", Roboto, Arial, sans-serif; font-size: 12px; }
body { background: var(--bg-main); color: var(--fg); display: flex; flex-direction: column; height: 100vh; overflow: hidden; }
.toolbar { background: var(--bg-menu); padding: 4px 10px; border-bottom: 1px solid var(--border); display: flex; gap: 10px; align-items: center; }
.toolbar svg { width: 14px; height: 14px; fill: currentColor; }
.workspace { display: flex; flex: 1; overflow: hidden; }
.sidebar { width: 300px; background: var(--bg-side); border-right: 1px solid var(--border); display: flex; flex-direction: column; }
.side-header { background: var(--bg-menu); padding: 4px 8px; font-weight: bold; border-bottom: 1px solid var(--border); }
.tree-container { flex: 1; overflow-y: auto; padding: 4px 0; }
.tree-container::-webkit-scrollbar, .main-panel::-webkit-scrollbar, .scene-inspector::-webkit-scrollbar { width: 10px; height: 10px; }
.tree-container::-webkit-scrollbar-thumb, .main-panel::-webkit-scrollbar-thumb, .scene-inspector::-webkit-scrollbar-thumb { background: #555; border: 2px solid var(--bg-side); border-radius: 5px; }
.tree-node { list-style: none; padding-left: 12px; }
.tree-node-content { display: flex; align-items: center; padding: 2px 4px; cursor: pointer; white-space: nowrap; }
.tree-node-content:hover { background: var(--bg-hover); }
.tree-node-content.active { background: var(--bg-sel); color: #fff; }
.tree-arrow { display: inline-block; width: 12px; text-align: center; font-size: 10px; transition: transform 0.1s; cursor: pointer; }
.tree-arrow.open { transform: rotate(90deg); }
.tree-icon { width: 16px; height: 16px; margin-right: 6px; display: flex; align-items: center; justify-content: center; }
.tree-icon svg { width: 100%; height: 100%; fill: currentColor; }
.tree-children { display: none; }
.tree-children.open { display: block; }
.main-area { flex: 1; display: flex; flex-direction: column; overflow: hidden; }
.tabs { display: flex; background: var(--bg-menu); border-bottom: 1px solid var(--border); }
.tab { padding: 4px 14px; background: var(--bg-side); border-right: 1px solid var(--border); cursor: pointer; }
.tab.active { background: var(--bg-main); border-bottom: 2px solid var(--kw); margin-bottom: -1px; }
.main-panel { flex: 1; overflow: auto; padding: 10px; background: var(--bg-main); display: none; }
.main-panel.active { display: flex; flex-direction: column; }
.pnl-code-area { font-family: Consolas, 'Courier New', monospace; font-size: 13px; white-space: pre; line-height: 1.4; }
.c-kw { color: var(--kw); }
.c-type { color: var(--type); }
.c-mth { color: var(--mth); }
.c-str { color: var(--str); }
.c-num { color: var(--num); }
.table { width: 100%; border-collapse: collapse; }
.table th, .table td { padding: 4px 8px; border: 1px solid var(--border); text-align: left; }
.table th { background: var(--bg-menu); }
.btn { background: #555; color: #fff; border: 1px solid #222; padding: 4px 12px; cursor: pointer; display: inline-flex; align-items: center; gap: 6px; border-radius: 2px; }
.btn svg { width: 12px; height: 12px; fill: currentColor; }
.btn:hover { background: #666; }
.btn-red { background: #883333; }
.btn-red:hover { background: #aa4444; }
.input { background: var(--input-bg); border: 1px solid var(--input-brd); color: #fff; padding: 3px 6px; width: 100%; outline: none; transition: background-color 0.3s; }
.input:focus { border-color: var(--kw); }
.input:disabled { opacity: 0.5; cursor: not-allowed; }
.form-group { margin-bottom: 8px; }
.form-label { display: block; margin-bottom: 4px; }
.srch { background: var(--input-bg); border: 1px solid var(--input-brd); color: #fff; padding: 2px 6px; width: 250px; margin-left: auto; outline: none; }
.srch:focus { border-color: var(--kw); }
.scene-manager { display: flex; flex: 1; overflow: hidden; margin: -10px; }
.scene-hierarchy { width: 350px; border-right: 1px solid var(--border); display: flex; flex-direction: column; }
.scene-hierarchy-header { background: var(--bg-menu); padding: 4px 8px; border-bottom: 1px solid var(--border); display: flex; flex-direction: column; gap: 5px; }
.scene-inspector { flex: 1; overflow-y: auto; background: var(--bg-main); }
.insp-obj-header { padding: 10px 15px; border-bottom: 1px solid var(--border); display: flex; align-items: center; gap: 10px; }
.insp-obj-header .input { flex: 1; font-size: 14px; font-weight: bold; }
.insp-comp { border-bottom: 1px solid var(--border); }
.insp-comp-header { background: var(--comp-hdr); font-weight: bold; padding: 6px 15px; display: flex; align-items: center; cursor: pointer; user-select: none; gap: 8px; }
.insp-comp-header:hover { background: #4a4a4a; }
.insp-comp-body { padding: 8px 15px; display: none; }
.insp-comp-body.open { display: block; }
.insp-row { display: flex; align-items: center; margin-bottom: 4px; }
.insp-lbl { width: 35%; overflow: hidden; text-overflow: ellipsis; white-space: nowrap; padding-right: 10px; color: #bbb; }
.insp-val { flex: 1; display: flex; gap: 5px; }
.vec-lbl { color: #888; margin-right: 2px; }
.ctrl-method-row { border: 1px solid var(--border); padding: 8px; margin-bottom: 5px; background: var(--input-bg); }
.ctrl-method-header { display: flex; align-items: center; justify-content: space-between; margin-bottom: 5px; }
.ctrl-method-args { display: flex; flex-direction: column; gap: 5px; padding-left: 10px; border-left: 2px solid var(--kw); }
.ctrl-res { margin-top: 5px; font-family: monospace; }
</style>
</head>
<body>
<div class="toolbar">
    <svg viewBox="0 0 24 24"><path d="M21 16.5c0 .38-.21.71-.53.88l-7.9 4.44c-.16.12-.36.18-.57.18s-.41-.06-.57-.18l-7.9-4.44A.991.991 0 0 1 3 16.5v-9c0-.38.21-.71.53-.88l7.9-4.44c.16-.12.36-.18.57-.18s.41.06.57.18l7.9 4.44c.32.17.53.5.53.88v9M12 4.15L6.04 7.5L12 10.85l5.96-3.35L12 4.15M5 15.91l6 3.38v-6.71L5 9.19v6.72m14 0v-6.72l-6 3.39v6.71l6-3.38z"/></svg>
    <span>Unity Explorer <span style="opacity:0.5;">()HTMLEND";

    html += std::to_string(asmCount);

    html += R"HTMLEND( assemblies)</span></span>
    <input type="text" class="srch" id="gSearch" placeholder="Search classes..." onkeyup="doSearch(event)">
</div>
<div class="workspace">
    <div class="sidebar">
        <div class="side-header">Project</div>
        <div class="tree-container" id="tree-root"></div>
    </div>
    <div class="main-area">
        <div class="tabs">
            <div class="tab active" onclick="switchTab('scene')" id="tab-scene">Scene</div>
            <div class="tab" onclick="switchTab('code')" id="tab-code">Inspector (Class)</div>
            <div class="tab" onclick="switchTab('inst')" id="tab-inst">Instances</div>
            <div class="tab" onclick="switchTab('ctrl')" id="tab-ctrl">Controller</div>
        </div>
        <div class="main-panel active" id="pnl-scene">
            <div class="scene-manager">
                <div class="scene-hierarchy">
                    <div class="scene-hierarchy-header">
                        <div style="display:flex;justify-content:space-between;align-items:center;">
                            <span>Hierarchy</span>
                            <button class="btn" onclick="loadScene()"><svg viewBox="0 0 24 24"><path d="M17.65 6.35C16.2 4.9 14.21 4 12 4c-4.42 0-7.99 3.58-7.99 8s3.57 8 7.99 8c3.73 0 6.84-2.55 7.73-6h-2.08c-.82 2.33-3.04 4-5.65 4-3.31 0-6-2.69-6-6s2.69-6 6-6c1.66 0 3.14.69 4.22 1.78L13 11h7V4l-2.35 2.35z"/></svg></button>
                        </div>
                        <input type="text" class="input" id="sceneSearch" placeholder="Search hierarchy..." onkeyup="filterScene()">
                    </div>
                    <div class="tree-container" id="scene-tree"></div>
                </div>
                <div class="scene-inspector" id="scene-insp">
                    <div style="padding:20px;text-align:center;color:#888;">Select a GameObject to inspect</div>
                </div>
            </div>
        </div>
        <div class="main-panel pnl-code-area" id="pnl-code"></div>
        <div class="main-panel" id="pnl-inst" style="padding:0">
            <div class="scene-manager">
                <div class="scene-hierarchy" style="width:100%;border:none;">
                    <div class="scene-hierarchy-header"><span>Instances</span></div>
                    <div class="tree-container" id="inst-list" style="padding:10px;">Loading...</div>
                </div>
            </div>
        </div>
        <div class="main-panel" id="pnl-ctrl" style="padding:0">
            <div class="scene-manager">
                <div class="scene-hierarchy" style="width:350px;">
                    <div class="scene-hierarchy-header"><span>Target Settings</span></div>
                    <div style="padding:10px;display:flex;flex-direction:column;gap:10px;">
                        <div><label class="form-label">Address (0x0 for Static)</label><input class="input" id="ctrl-addr" value="0x0"></div>
                        <div><label class="form-label">Assembly</label><input class="input" id="ctrl-asm"></div>
                        <div><label class="form-label">Namespace</label><input class="input" id="ctrl-ns"></div>
                        <div><label class="form-label">Class</label><input class="input" id="ctrl-cls"></div>
                        <button class="btn" style="justify-content:center" onclick="loadController()"><svg viewBox="0 0 24 24"><path d="M12 4.5C7 4.5 2.73 7.61 1 12c1.73 4.39 6 7.5 11 7.5s9.27-3.11 11-7.5c-1.73-4.39-6-7.5-11-7.5zM12 17c-2.76 0-5-2.24-5-5s2.24-5 5-5 5 2.24 5 5-2.24 5-5 5zm0-8c-1.66 0-3 1.34-3 3s1.34 3 3 3 3-1.34 3-3-1.34-3-3-3z"/></svg> Fetch Data</button>
                    </div>
                </div>
                <div class="scene-inspector" id="ctrl-insp">
                    <div style="padding:20px;text-align:center;color:#888;">Configure target and fetch data</div>
                </div>
            </div>
        </div>
    </div>
</div>

<svg style="display:none;" xmlns="http://www.w3.org/2000/svg">
    <symbol id="svg-asm" viewBox="0 0 24 24"><path d="M4 6h16v2H4zm0 5h16v2H4zm0 5h16v2H4z"/></symbol>
    <symbol id="svg-ns" viewBox="0 0 24 24"><path d="M10 4H4c-1.1 0-1.99.9-1.99 2L2 18c0 1.1.9 2 2 2h16c1.1 0 2-.9 2-2V8c0-1.1-.9-2-2-2h-8z"/></symbol>
    <symbol id="svg-cls" viewBox="0 0 24 24"><path d="M12 2L2 7l10 5l10-5zm0 10.5l-10-5v5.1l10 5l10-5v-5.1z"/></symbol>
    <symbol id="svg-go" viewBox="0 0 24 24"><path d="M21 16.5c0 .38-.21.71-.53.88l-7.9 4.44c-.16.12-.36.18-.57.18s-.41-.06-.57-.18l-7.9-4.44A.991.991 0 0 1 3 16.5v-9c0-.38.21-.71.53-.88l7.9-4.44c.16-.12.36-.18.57-.18s.41.06.57.18l7.9 4.44c.32.17.53.5.53.88v9M12 4.15L6.04 7.5L12 10.85l5.96-3.35L12 4.15M5 15.91l6 3.38v-6.71L5 9.19v6.72m14 0v-6.72l-6 3.39v6.71l6-3.38z"/></symbol>
    <symbol id="svg-comp" viewBox="0 0 24 24"><path d="M14 2H6c-1.1 0-1.99.9-1.99 2L4 20c0 1.1.89 2 1.99 2H18c1.1 0 2-.9 2-2V8l-6-6zm2 16H8v-2h8v2zm0-4H8v-2h8v2zm-3-5V3.5L18.5 9H13z"/></symbol>
    <symbol id="svg-invoke" viewBox="0 0 24 24"><path d="M8 5v14l11-7z"/></symbol>
    <symbol id="svg-save" viewBox="0 0 16 16"><path d="M2 1a2 2 0 0 0-2 2v10a2 2 0 0 0 2 2h12a2 2 0 0 0 2-2V4.207a1 1 0 0 0-.293-.707l-2.5-2.5A1 1 0 0 0 10.5 1H2zm3 2h6v3H5V3zm7 10H4V9h8v4z"/></symbol>
</svg>

<script>
let curAsm = "", curNs = "", curCls = "";
let clsCache = {};
let allClsCache = null;
let fullSceneData = [];
let sceneRoots = [];

async function init() {
    let asms = await fetch('/api/assemblies').then(r => r.json());
    let html = '';
    for (let a of asms)
        html += `<ul class="tree-node"><div class="tree-node-content" onclick="toggleAsm(this,'${a}')"><span class="tree-arrow">&#9654;</span><div class="tree-icon"><svg><use href="#svg-asm"></use></svg></div><span>${a}</span></div><div class="tree-children"></div></ul>`;
    document.getElementById('tree-root').innerHTML = html;
    loadScene();
}

async function toggleAsm(el, asm) {
    let kids = el.nextElementSibling;
    let arrow = el.querySelector('.tree-arrow');
    if (kids.classList.contains('open')) { kids.classList.remove('open'); arrow.classList.remove('open'); return; }
    if (!kids.innerHTML) {
        let classes = await fetch(`/api/classes?a=${encodeURIComponent(asm)}`).then(r => r.json());
        let byNs = {};
        for (let c of classes) { let ns = c.ns || ''; if (!byNs[ns]) byNs[ns] = []; byNs[ns].push(c); }
        let html = '';
        for (let ns of Object.keys(byNs).sort()) {
            let label = ns || '(global)';
            html += `<ul class="tree-node"><div class="tree-node-content" onclick="toggleNs(this)"><span class="tree-arrow">&#9654;</span><div class="tree-icon"><svg><use href="#svg-ns"></use></svg></div><span>${label}</span></div><div class="tree-children">`;
            for (let c of byNs[ns].sort((a,b) => a.name.localeCompare(b.name)))
                html += `<div class="tree-node-content" onclick="loadCls(this,'${asm}','${c.ns}','${c.name}')"><span class="tree-arrow"></span><div class="tree-icon" style="color:var(--type)"><svg><use href="#svg-cls"></use></svg></div><span>${c.name}</span></div>`;
            html += `</div></ul>`;
        }
        kids.innerHTML = html;
    }
    kids.classList.add('open');
    arrow.classList.add('open');
}

function toggleNs(el) {
    el.nextElementSibling.classList.toggle('open');
    el.querySelector('.tree-arrow').classList.toggle('open');
}

function esc(s) {
    if (!s) return "";
    return String(s).replace(/&/g,"&amp;").replace(/</g,"&lt;").replace(/>/g,"&gt;").replace(/"/g,"&quot;").replace(/'/g,"&#039;");
}

async function loadCls(el, asm, ns, name) {
    document.querySelectorAll('#tree-root .tree-node-content').forEach(e => e.classList.remove('active'));
    if (el) el.classList.add('active');
    curAsm = asm; curNs = ns; curCls = name;

    let d = await fetch(`/api/class?a=${encodeURIComponent(asm)}&ns=${encodeURIComponent(ns)}&n=${encodeURIComponent(name)}`).then(r => r.json());
    clsCache = d;

    let indent = d.ns ? '\t\t' : '\t';
    let code = `<span class="c-cm">// Assembly: ${esc(asm)}</span>\n`;
    if (d.ns) code += `<span class="c-kw">namespace</span> <span class="c-type">${esc(d.ns)}</span>\n{\n`;
    code += (d.ns ? '\t' : '') + `<span class="c-kw">public class</span> <span class="c-type">${esc(d.name)}</span>`;
    if (d.parent) code += ` : <span class="c-type">${esc(d.parent)}</span>`;
    code += '\n' + (d.ns ? '\t' : '') + '{\n';

    for (let f of d.fields || []) {
        code += `${indent}<span class="c-cm">// Offset: 0x${f.off.toString(16).toUpperCase()}</span>\n`;
        code += `${indent}<span class="c-kw">public</span> ${f.s?'<span class="c-kw">static</span> ':''}<span class="c-type">${esc(f.type)}</span> ${esc(f.name)};\n\n`;
    }
    for (let p of d.props || []) {
        code += `${indent}<span class="c-kw">public</span> <span class="c-type">${esc(p.type)}</span> ${esc(p.name)} { ${p.g?'<span class="c-kw">get</span>; ':''}${p.s?'<span class="c-kw">set</span>; ':''}}\n\n`;
    }
    for (let m of d.methods || []) {
        let prm = (m.params || []).map(p => `<span class="c-type">${esc(p.t)}</span> ${p.n}`).join(', ');
        code += `${indent}<span class="c-cm">// Address: 0x${m.addr}</span>\n`;
        code += `${indent}<span class="c-kw">public</span> ${m.s?'<span class="c-kw">static</span> ':''}<span class="c-type">${esc(m.ret)}</span> <span class="c-mth">${esc(m.name)}</span>(${prm}) { }\n\n`;
    }

    code += (d.ns ? '\t' : '') + '}\n';
    if (d.ns) code += '}\n';

    document.getElementById('pnl-code').innerHTML = code;
    switchTab('code');
    loadInstances();
}

async function loadInstances() {
    document.getElementById('inst-list').innerHTML = 'Loading instances...';
    let d = await fetch(`/api/instances?a=${encodeURIComponent(curAsm)}&ns=${encodeURIComponent(curNs)}&n=${encodeURIComponent(curCls)}`).then(r => r.json());
    if (d.error) { document.getElementById('inst-list').innerHTML = d.error; return; }
    let h = `<div style="margin-bottom:10px;font-size:14px;">Instances found: ${d.instances.length}</div><table class="table"><tr><th>Address</th><th>Name</th><th>Actions</th></tr>`;
    for (let i of d.instances)
        h += `<tr><td><span class="c-num">0x${i.addr}</span></td><td><span class="c-str">"${esc(i.name)}"</span></td><td><button class="btn" onclick="openController('0x${i.addr}')"><svg viewBox="0 0 24 24"><path d="M12 4.5C7 4.5 2.73 7.61 1 12c1.73 4.39 6 7.5 11 7.5s9.27-3.11 11-7.5c-1.73-4.39-6-7.5-11-7.5zM12 17c-2.76 0-5-2.24-5-5s2.24-5 5-5 5 2.24 5 5-2.24 5-5 5zm0-8c-1.66 0-3 1.34-3 3s1.34 3 3 3 3-1.34 3-3-1.34-3-3-3z"/></svg> Open Controller</button></td></tr>`;
    document.getElementById('inst-list').innerHTML = h + '</table>';
}

function openController(addr) {
    document.getElementById('ctrl-addr').value = addr;
    document.getElementById('ctrl-asm').value = curAsm;
    document.getElementById('ctrl-ns').value = curNs;
    document.getElementById('ctrl-cls').value = curCls;
    switchTab('ctrl');
    loadController();
}

async function loadController() {
    let addr = document.getElementById('ctrl-addr').value;
    let asm  = document.getElementById('ctrl-asm').value;
    let ns   = document.getElementById('ctrl-ns').value;
    let cls  = document.getElementById('ctrl-cls').value;
    document.getElementById('ctrl-insp').innerHTML = '<div style="padding:20px;text-align:center;">Fetching data...</div>';

    let d = await fetch(`/api/controller/inspect?addr=${addr}&asm=${encodeURIComponent(asm)}&ns=${encodeURIComponent(ns)}&cls=${encodeURIComponent(cls)}`).then(r => r.json());
    if (!d.name) { document.getElementById('ctrl-insp').innerHTML = '<div style="padding:20px;text-align:center;color:#f44336;">Invalid target or class not found.</div>'; return; }

    let h = `<div class="insp-obj-header"><div class="tree-icon"><svg><use href="#svg-cls"></use></svg></div><span class="input">${esc(d.name)} (${addr})</span></div>`;
    if (d.fields?.length) h += buildCompUI("Fields & Properties", false, d.fields, addr, 'instance', true);

    if (d.methods?.length) {
        let mid = Math.random().toString(36).substr(2, 9);
        h += `<div class="insp-comp"><div class="insp-comp-header" onclick="toggleComp('${mid}')"><div class="tree-icon"><svg><use href="#svg-invoke"></use></svg></div><span>Methods</span></div><div class="insp-comp-body open" id="${mid}">`;
        d.methods.forEach((m, idx) => {
            h += `<div class="ctrl-method-row"><div class="ctrl-method-header"><span style="font-weight:bold;color:var(--mth);">${m.s?'[S] ':''}${esc(m.name)}</span><span style="color:var(--type);font-size:10px;">-> ${esc(m.ret)}</span></div>`;
            if (m.params?.length) {
                h += `<div class="ctrl-method-args">`;
                m.params.forEach(p => { h += `<div style="display:flex;align-items:center;gap:5px;"><span style="width:60px;color:#bbb;font-size:11px;">${esc(p.t)}</span><input type="text" class="input mth-arg-${idx}" data-type="${esc(p.t)}" placeholder="${esc(p.n)}"></div>`; });
                h += `</div>`;
            }
            h += `<div style="margin-top:8px;display:flex;justify-content:space-between;align-items:center;"><button class="btn" onclick="execMethod('${asm}','${ns}','${cls}','${m.name}',${m.s},'${addr}',${idx})"><svg viewBox="0 0 24 24"><path d="M8 5v14l11-7z"/></svg> Call</button><div class="ctrl-res" id="mres-${idx}"></div></div></div>`;
        });
        h += `</div></div>`;
    }
    document.getElementById('ctrl-insp').innerHTML = h;
}

async function execMethod(asm, ns, cls, mName, isStatic, addr, idx) {
    let args = Array.from(document.querySelectorAll(`.mth-arg-${idx}`)).map(el => ({ t: el.getAttribute('data-type'), v: el.value }));
    let res = document.getElementById(`mres-${idx}`);
    res.innerHTML = '<span style="color:#aaa">...</span>';
    let d = await fetch('/api/invoke', { method:'POST', headers:{'Content-Type':'application/json'}, body: JSON.stringify({ asm, ns, cls, method: mName, static: isStatic, instance: addr, args }) }).then(r => r.json());
    res.innerHTML = d.ok ? `<span style="color:var(--str)">${esc(d.value)}</span>` : `<span style="color:#f44336">${esc(d.error)}</span>`;
}

function switchTab(id) {
    document.querySelectorAll('.tab').forEach(t => t.classList.remove('active'));
    document.querySelectorAll('.main-panel').forEach(p => p.classList.remove('active'));
    document.getElementById(`tab-${id}`).classList.add('active');
    document.getElementById(`pnl-${id}`).classList.add('active');
}

async function doSearch(e) {
    if (e.key !== 'Enter') return;
    let q = document.getElementById('gSearch').value.toLowerCase();
    if (!q) { init(); return; }
    if (!allClsCache) allClsCache = await fetch('/api/allclasses').then(r => r.json());
    let res = allClsCache.filter(c => c.name.toLowerCase().includes(q) || c.ns?.toLowerCase().includes(q));
    let byAsm = {};
    for (let c of res) { if (!byAsm[c.a]) byAsm[c.a] = []; byAsm[c.a].push(c); }
    let html = '';
    for (let a of Object.keys(byAsm).sort()) {
        html += `<ul class="tree-node"><div class="tree-node-content" onclick="toggleAsm(this,'${a}')"><span class="tree-arrow open">&#9654;</span><div class="tree-icon"><svg><use href="#svg-asm"></use></svg></div><span>${a}</span></div><div class="tree-children open">`;
        let byNs = {};
        for (let c of byAsm[a]) { let ns = c.ns || ''; if (!byNs[ns]) byNs[ns] = []; byNs[ns].push(c); }
        for (let ns of Object.keys(byNs).sort()) {
            html += `<ul class="tree-node"><div class="tree-node-content" onclick="toggleNs(this)"><span class="tree-arrow open">&#9654;</span><div class="tree-icon"><svg><use href="#svg-ns"></use></svg></div><span>${ns||'(global)'}</span></div><div class="tree-children open">`;
            for (let c of byNs[ns].sort((x,y) => x.name.localeCompare(y.name)))
                html += `<div class="tree-node-content" onclick="loadCls(this,'${a}','${c.ns}','${c.name}')"><span class="tree-arrow"></span><div class="tree-icon" style="color:var(--type)"><svg><use href="#svg-cls"></use></svg></div><span>${c.name}</span></div>`;
            html += `</div></ul>`;
        }
        html += `</div></ul>`;
    }
    document.getElementById('tree-root').innerHTML = html;
}

async function loadScene() {
    fullSceneData = await fetch('/api/scene').then(r => r.json());
    filterScene();
}

function filterScene() {
    let q = document.getElementById('sceneSearch').value.toLowerCase();
    let map = {};
    sceneRoots = [];
    for (let g of fullSceneData) {
        if (!map[g.addr]) map[g.addr] = { ...g, children: [] };
        else Object.assign(map[g.addr], g);
        if (!g.parent || g.parent === "0") sceneRoots.push(map[g.addr]);
        else { if (!map[g.parent]) map[g.parent] = { children: [] }; map[g.parent].children.push(map[g.addr]); }
    }
    document.getElementById('scene-tree').innerHTML = sceneRoots.map(n => buildSceneNode(n, q)).filter(Boolean).join('');
}

function buildSceneNode(n, q) {
    let matches = !q || n.name.toLowerCase().includes(q);
    let childHtml = n.children.map(c => buildSceneNode(c, q)).filter(Boolean).join('');
    if (!matches && !childHtml) return '';
    let open = q || n.children.length > 0;
    let h = `<ul class="tree-node"><div class="tree-node-content" onclick="inspectGO(this,'${n.addr}')"><span class="tree-arrow ${open?'open':''}" onclick="toggleNs(this.parentElement);event.stopPropagation()">${n.children.length?'&#9654;':''}</span><div class="tree-icon"><svg><use href="#svg-go"></use></svg></div><span style="color:${n.active?'var(--fg)':'#777'}">${esc(n.name)}</span></div>`;
    if (n.children.length) h += `<div class="tree-children ${open?'open':''}">${childHtml}</div>`;
    return h + '</ul>';
}

async function inspectGO(el, addr) {
    document.querySelectorAll('#scene-tree .tree-node-content').forEach(e => e.classList.remove('active'));
    if (el) el.classList.add('active');
    let d = await fetch(`/api/scene/inspect?addr=${addr}`).then(r => r.json());
    if (!d.addr) return;

    let h = `<div class="insp-obj-header">
        <input type="checkbox" ${d.active?'checked':''} onchange="updateGO('${d.addr}','gameobject','active',this.checked)">
        <div class="tree-icon"><svg><use href="#svg-go"></use></svg></div>
        <input type="text" class="input" value="${esc(d.name)}" onchange="updateGO('${d.addr}','gameobject','name',this.value)">
    </div>`;
    if (d.transform)
        h += buildCompUI("Transform", true, [
            { lbl:"Position", id:"p", val:d.transform.p, type:"UnityEngine.Vector3", canWrite:true },
            { lbl:"Rotation", id:"r", val:d.transform.r, type:"UnityEngine.Vector3", canWrite:true },
            { lbl:"Scale",    id:"s", val:d.transform.s, type:"UnityEngine.Vector3", canWrite:true }
        ], d.transform.addr, 'transform', true);
    document.getElementById('scene-insp').innerHTML = h;
}

function buildCompUI(title, isTrans, rows, targetAddr, targetType, withSave) {
    let uid = Math.random().toString(36).substr(2, 9);
    let h = `<div class="insp-comp"><div class="insp-comp-header" onclick="toggleComp('${uid}')"><div class="tree-icon"><svg><use href="#svg-comp"></use></svg></div><span>${esc(title)}</span></div><div class="insp-comp-body open" id="${uid}">`;
    for (let r of rows) {
        let da = `data-addr="${targetAddr}" data-target-type="${targetType}" data-prop-id="${r.id||r.name||r.lbl}" data-val-type="${r.type}" data-is-prop="${r.isProp?'true':'false'}"`;
        let dis = r.canWrite === false ? 'disabled' : '';
        h += `<div class="insp-row"><div class="insp-lbl" title="${esc(r.name||r.lbl)}">${esc(r.name||r.lbl)}</div><div class="insp-val">`;
        if (isTrans || r.type === "UnityEngine.Vector3" || r.type === "Vector3") {
            let v = Array.isArray(r.val) ? r.val : JSON.parse(r.val || "[0,0,0]");
            ['X','Y','Z'].forEach((lbl,i) => { h += `<div style="flex:1;display:flex"><span class="vec-lbl">${lbl}</span><input class="input vec-input" type="number" step="any" value="${v[i]}" ${da} ${dis}></div>`; });
        } else if (r.type === "UnityEngine.Color" || r.type === "Color") {
            let v = JSON.parse(r.val || "[1,1,1,1]");
            let hex = "#" + ((1<<24)+(Math.round(v[0]*255)<<16)+(Math.round(v[1]*255)<<8)+Math.round(v[2]*255)).toString(16).slice(1).padStart(6,'0');
            h += `<input type="color" class="input sing-input" value="${hex}" style="padding:0;height:20px" ${da} ${dis}>`;
        } else if (r.type === "System.Boolean" || r.type === "Boolean" || r.type === "bool") {
            h += `<input type="checkbox" class="sing-input" ${r.val===true||r.val==="true"?'checked':''} ${da} ${dis}>`;
        } else {
            let isNum = ["System.Int32","Int32","int","System.Single","Single","float"].includes(r.type);
            let sv = String(r.val).replace(/"/g,"&quot;");
            if (r.type==="System.String"||r.type==="String"||r.type==="string")
                if (sv.startsWith('&quot;') && sv.endsWith('&quot;')) sv = sv.slice(6, -6);
            h += `<input class="input sing-input" ${isNum?'type="number" step="any"':'type="text"'} value="${sv}" ${da} ${dis}>`;
        }
        if (withSave) h += `<button class="btn" style="margin-left:5px" onclick="handleManualSave(this)" ${dis}><svg><use href="#svg-save"></use></svg></button>`;
        h += `</div></div>`;
    }
    return h + '</div></div>';
}

function toggleComp(id) { document.getElementById(id).classList.toggle('open'); }

function handleManualSave(btn) {
    let el = btn.parentElement.querySelector('.vec-input') || btn.parentElement.querySelector('.sing-input');
    if (el) processSave(el);
}

function processSave(el) {
    let p = el.classList.contains('vec-input') ? el.parentElement.parentElement : el.parentElement;
    let val = '';
    if (el.classList.contains('vec-input')) {
        let ins = p.querySelectorAll('input.vec-input');
        val = `[${ins[0].value},${ins[1].value},${ins[2].value}]`;
        el = ins[0];
    } else if (el.type === 'color') {
        let h = el.value;
        val = `[${parseInt(h.substr(1,2),16)/255},${parseInt(h.substr(3,2),16)/255},${parseInt(h.substr(5,2),16)/255},1]`;
    } else if (el.type === 'checkbox') {
        val = el.checked ? 'true' : 'false';
    } else {
        val = el.value;
    }
    dispatchUpdate(el, val);
    let orig = el.style.backgroundColor;
    el.style.backgroundColor = '#2c5d87';
    setTimeout(() => { el.style.backgroundColor = orig; }, 300);
}

function dispatchUpdate(el, val) {
    let type = el.getAttribute('data-target-type');
    let addr = el.getAttribute('data-addr');
    let prop = el.getAttribute('data-prop-id');
    let vtype = el.getAttribute('data-val-type');
    let isProp = el.getAttribute('data-is-prop') === 'true';
    if (type === 'transform') updateGO(addr, 'transform', prop, val);
    else if (type === 'instance') updateInstField(addr, prop, vtype, isProp, val);
}

async function updateGO(addr, type, prop, val) {
    await fetch('/api/scene/update', { method:'POST', headers:{'Content-Type':'application/json'}, body: JSON.stringify({ addr, type, prop, val: String(val) }) });
}

async function updateInstField(addr, name, ftype, isProp, val) {
    let asm = document.getElementById('ctrl-asm').value;
    let ns  = document.getElementById('ctrl-ns').value;
    let cls = document.getElementById('ctrl-cls').value;
    await fetch('/api/instance/update', { method:'POST', headers:{'Content-Type':'application/json'}, body: JSON.stringify({ addr, asm, ns, cls, name, ftype, isProp, val: String(val) }) });
}

init();
</script>
</body>
</html>)HTMLEND";
    return html;
}