#include <WiFi.h>
#include <WebServer.h>

const char* ssid = "Vishal";
const char* password = "11111111";

WebServer server(80);

const int S1pin = 34;
const int S2pin = 35;
const int S3pin = 32;

String webpage = R"=====(

<!DOCTYPE html>
<html>

<head>

<title>Nadi Pulse Monitor</title>

<script src="https://cdn.jsdelivr.net/npm/chart.js"></script>

<style>

body{
background:#0f0f0f;
color:white;
font-family:Arial;
text-align:center;
}

canvas{
background:#111;
border:2px solid #333;
}

button{
padding:10px 20px;
margin:10px;
font-size:16px;
cursor:pointer;
}

#start{background:green;color:white}
#stop{background:red;color:white}
#download{background:blue;color:white}

</style>

</head>

<body>

<h2>Nadi Pulse Real Time Graph</h2>

<canvas id="chart" width="1000" height="400"></canvas>

<br>

<button id="start">Start</button>
<button id="stop">Stop</button>
<button id="download">Download CSV</button>

<script>

let running=false
let stored=[]

const ctx=document.getElementById("chart")

const chart=new Chart(ctx,{
type:"line",
data:{
labels:[],
datasets:[
{label:"S1",borderColor:"red",data:[],tension:0.3},
{label:"S2",borderColor:"green",data:[],tension:0.3},
{label:"S3",borderColor:"yellow",data:[],tension:0.3}
]
},
options:{
animation:false,
responsive:false,
scales:{
y:{min:0,max:4095}
}
}
})

document.getElementById("start").onclick=function(){
running=true
}

document.getElementById("stop").onclick=function(){
running=false
}

async function readData(){

if(!running) return

let response=await fetch("/data")
let data=await response.json()

let t=(Date.now()/1000).toFixed(2)

chart.data.labels.push(t)

chart.data.datasets[0].data.push(data.S1)
chart.data.datasets[1].data.push(data.S2)
chart.data.datasets[2].data.push(data.S3)

stored.push([t,data.S1,data.S2,data.S3])

chart.update('none')

}

setInterval(readData,33)

document.getElementById("download").onclick=function(){

let csv="Time,S1,S2,S3\n"

stored.forEach(r=>{
csv+=r.join(",")+"\n"
})

let blob=new Blob([csv])

let a=document.createElement("a")

a.href=URL.createObjectURL(blob)

a.download="nadi_data.csv"

a.click()

}

</script>

</body>

</html>

)=====";


void handleRoot()
{
server.send(200,"text/html",webpage);
}

void handleData()
{

int S1 = analogRead(S1pin);
int S2 = analogRead(S2pin);
int S3 = analogRead(S3pin);

String json = "{";
json += "\"S1\":" + String(S1) + ",";
json += "\"S2\":" + String(S2) + ",";
json += "\"S3\":" + String(S3);
json += "}";

server.send(200,"application/json",json);

}

void setup()
{

Serial.begin(115200);

WiFi.begin(ssid,password);

Serial.print("Connecting");

while(WiFi.status()!=WL_CONNECTED)
{
delay(500);
Serial.print(".");
}

Serial.println();
Serial.println("Connected");

Serial.print("Open Browser: ");
Serial.println(WiFi.localIP());

server.on("/",handleRoot);
server.on("/data",handleData);

server.begin();

}

void loop()
{
server.handleClient();
}