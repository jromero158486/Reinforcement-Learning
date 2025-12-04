<!DOCTYPE html>
<html>
<head>
<meta charset="utf-8">
<title>Robot Explorer Dashboard</title>

<style>
body {
  background:#111; color:#eee; font-family:Arial;
  text-align:center; padding:10px;
}

button {
  padding:15px 25px; margin:8px; font-size:20px;
  border-radius:10px; cursor:pointer; 
}

.modeBtn { background:#0af; }
.manualBtn { background:#222; color:#0af; }
.stopBtn { background:#c00; color:white; }

#radarCanvas {
  background:#000; margin-top:20px;
  border:2px solid #444;
}

.qcell {
  width:70px; height:28px; 
  display:inline-block; margin:3px;
  border-radius:6px; line-height:28px;
  color:#000; font-weight:bold;
}
</style>
</head>

<body>

<h1>🤖 Robot RL Explorer — Dashboard</h1>

<button id="connectBtn" class="modeBtn">Conectar Bluetooth</button>
<button id="manualBtn" class="manualBtn">Manual</button>
<button id="autoBtn" class="modeBtn">Autonomous RL</button>
<button id="stopBtn" class="stopBtn">STOP</button>

<h2>Manual Control</h2>
<button onclick="sendCmd('U')">⬆</button><br>
<button onclick="sendCmd('L')">⬅</button>
<button onclick="sendCmd('S')">⏹</button>
<button onclick="sendCmd('R')">➡</button><br>
<button onclick="sendCmd('D')">⬇</button>

<h2>Radar</h2>
<canvas id="radarCanvas" width="300" height="300"></canvas>

<h2>Q-Table</h2>
<div id="qtable"></div>

<h2>Log</h2>
<pre id="logBox" style="background:#222; padding:10px; width:90%; margin:auto; height:200px; overflow:auto;"></pre>

<script>
let port, writer;
let reader;
let buffer = "";

// ==================================================
//                BLUETOOTH SERIAL
// ==================================================

async function connect() {
  port = await navigator.serial.requestPort();
  await port.open({ baudRate:9600 });

  writer = port.writable.getWriter();
  reader = port.readable.getReader();

  readLoop();
}

document.getElementById("connectBtn").onclick = connect;

function sendCmd(c){
  if (!writer) return;
  writer.write(new TextEncoder().encode(c));
}

// ==================================================
//               MODO MANUAL / AUTÓNOMO
// ==================================================

document.getElementById("manualBtn").onclick = () => sendCmd("M");
document.getElementById("autoBtn").onclick   = () => sendCmd("A");
document.getElementById("stopBtn").onclick   = () => sendCmd("S");

// ==================================================
//              RADAR VISUAL
// ==================================================

const radar = document.getElementById("radarCanvas");
const ctx = radar.getContext("2d");

function drawRadar(dist){
  ctx.fillStyle="#000";
  ctx.fillRect(0,0,300,300);

  ctx.strokeStyle="#0f0";
  ctx.beginPath();
  ctx.moveTo(150,150);

  let angle = 90 * Math.PI/180;
  let r = Math.min(dist*2, 250);

  ctx.lineTo(150 + r*Math.cos(angle), 150 - r*Math.sin(angle));
  ctx.stroke();

  ctx.fillStyle="#0f0";
  ctx.beginPath();
  ctx.arc(150,150,5,0,6.28);
  ctx.fill();
}

// ==================================================
//               Q-TABLE VISUAL
// ==================================================

function renderQ(q){
  let html="";
  for(let s=0;s<q.length;s++){
    html += "<div>Estado "+s+": ";
    for(let a=0;a<q[s].length;a++){
      let v = q[s][a];
      let color = v>0 ? `rgb(${255-v*20},255,150)` : `rgb(255,${255+v*20},150)`;
      html += `<span class='qcell' style='background:${color}'>${v.toFixed(1)}</span>`;
    }
    html += "</div>";
  }
  document.getElementById("qtable").innerHTML = html;
}

// ==================================================
//                 LECTURA SERIAL
// ==================================================

function log(msg){
  let box = document.getElementById("logBox");
  box.textContent += msg+"\n";
  box.scrollTop = box.scrollHeight;
}

async function readLoop(){
  while(true){
    const {value,done} = await reader.read();
    if (done) break;

    buffer += new TextDecoder().decode(value);
    let lines = buffer.split("\n");
    buffer = lines.pop();

    for (let ln of lines){
      ln = ln.trim();
      if (!ln.startsWith("{")) continue;

      try {
        let data = JSON.parse(ln);

        drawRadar(data.dist);

        log(`dist=${data.dist} | state=${data.state} | action=${data.action} | reward=${data.reward}`);

      } catch(e){}
    }
  }
}
</script>

</body>
</html>
