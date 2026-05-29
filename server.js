const express = require("express");
const http = require("http");
const { WebSocketServer } = require("ws");
const path = require("path");

const app = express();
const server = http.createServer(app);
const wss = new WebSocketServer({ server });

app.use(express.static(path.join(__dirname)));

let state = { speed: 0, brake: false };
let arduino = null;
let dashboard = null;

wss.on("connection", (ws, req) => {
  const type = new URL(req.url, "http://localhost").searchParams.get("type");

  console.log(
    `[ws] ${type || "unknown"} connected from ${req.socket.remoteAddress}`,
  );

  if (type === "arduino") {
    arduino = ws;
    ws.on("close", () => {
      console.log("[ws] arduino disconnected");
      arduino = null;
    });
  } else {
    dashboard = ws;
    console.log("[ws] dashboard state sent", state);
    ws.send(JSON.stringify(state));
    ws.on("message", (msg) => {
      try {
        const data = JSON.parse(msg);
        if (data.speed !== undefined && data.speed !== state.speed) {
          console.log(`[ws] speed changed: ${state.speed} -> ${data.speed}`);
          state.speed = data.speed;
        }
        if (data.brake !== undefined && data.brake !== state.brake) {
          console.log(`[ws] brake changed: ${state.brake} -> ${data.brake}`);
          state.brake = data.brake;
        }
        if (arduino && arduino.readyState === 1)
          arduino.send(JSON.stringify(state));
      } catch {}
    });
    ws.on("close", () => {
      console.log("[ws] dashboard disconnected");
      dashboard = null;
    });
  }
});

server.listen(3000, () =>
  console.log("Server running on http://localhost:3000"),
);
