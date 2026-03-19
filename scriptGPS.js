console.log("🗺️ GPS Tracker script loaded");

// ================= MAP INITIALIZATION =================
let map;
let marker;
let polyline;
let locations = [];
let totalDistance = 0;

function initMap() {
  console.log("Initializing map...");
  
  // Create map centered on Tunisia (default)
  map = L.map('map').setView([35.8567, 10.5951], 13);

  // Add OpenStreetMap tiles
  L.tileLayer('https://{s}.tile.openstreetmap.org/{z}/{x}/{y}.png', {
    attribution: '© OpenStreetMap contributors',
    maxZoom: 19,
    minZoom: 2
  }).addTo(map);

  console.log("✅ Map initialized");
}

// ================= HTML ELEMENTS =================
const latEl = document.getElementById("latitude");
const lngEl = document.getElementById("longitude");
const altEl = document.getElementById("altitude");
const satEl = document.getElementById("satellites");
const hdopEl = document.getElementById("hdop");
const lastUpdateEl = document.getElementById("lastUpdate");

let gpsData = {
  lat: null,
  lng: null,
  alt: null,
  sat: null,
  hdop: null
};

// ================= MQTT CONNECTION =================
console.log("Setting up MQTT connection...");

const brokerIP = "localhost";  // Change to your IP if on different machine
const espId = "Na7la";
const wsUrl = `ws://${brokerIP}:9001`;

console.log("Connecting to MQTT broker at: " + wsUrl);

const client = mqtt.connect(wsUrl, {
  clientId: 'gps_web_' + Math.random().toString(16).substr(2, 8),
  reconnectPeriod: 3000,
  clean: true
});

// ================= MQTT EVENTS =================
client.on("connect", () => {
  console.log("✅ Connected to MQTT broker at " + wsUrl);

  // Subscribe to GPS topics
  client.subscribe(`${espId}/gps/latitude`, (err) => {
    if (err) console.error("Subscribe error:", err);
  });
  client.subscribe(`${espId}/gps/longitude`, (err) => {
    if (err) console.error("Subscribe error:", err);
  });
  client.subscribe(`${espId}/gps/altitude`, (err) => {
    if (err) console.error("Subscribe error:", err);
  });
  client.subscribe(`${espId}/gps/satellites`, (err) => {
    if (err) console.error("Subscribe error:", err);
  });
  client.subscribe(`${espId}/gps/hdop`, (err) => {
    if (err) console.error("Subscribe error:", err);
  });

  console.log("📨 Subscribed to all GPS topics");
});

client.on("message", (topic, message) => {
  const msg = message.toString();
  console.log(`📨 Received: ${topic} = ${msg}`);

  if (topic === `${espId}/gps/latitude`) {
    gpsData.lat = parseFloat(msg);
    if (!isNaN(gpsData.lat)) {
      latEl.innerText = gpsData.lat.toFixed(6);
    }
  }

  if (topic === `${espId}/gps/longitude`) {
    gpsData.lng = parseFloat(msg);
    if (!isNaN(gpsData.lng)) {
      lngEl.innerText = gpsData.lng.toFixed(6);
    }
  }

  if (topic === `${espId}/gps/altitude`) {
    gpsData.alt = parseFloat(msg);
    if (!isNaN(gpsData.alt)) {
      altEl.innerText = gpsData.alt.toFixed(2) + " m";
    }
  }

  if (topic === `${espId}/gps/satellites`) {
    gpsData.sat = parseInt(msg);
    if (!isNaN(gpsData.sat)) {
      satEl.innerText = gpsData.sat;
    }
  }

  if (topic === `${espId}/gps/hdop`) {
    gpsData.hdop = parseFloat(msg);
    if (!isNaN(gpsData.hdop)) {
      hdopEl.innerText = gpsData.hdop.toFixed(1);
    }
  }

  // Update map when we have both lat and lng
  if (gpsData.lat && gpsData.lng) {
    updateMap();
    updateLastUpdate();
  }
});

client.on("error", (err) => {
  console.error("❌ MQTT Error:", err);
});

client.on("offline", () => {
  console.log("⚠️ MQTT Offline");
});

client.on("reconnect", () => {
  console.log("🔄 MQTT Reconnecting...");
});

client.on("close", () => {
  console.log("⚠️ MQTT Connection closed");
});

// ================= UPDATE MAP =================
function updateMap() {
  if (!gpsData.lat || !gpsData.lng) return;

  const latLng = [gpsData.lat, gpsData.lng];

  // Remove old marker
  if (marker) {
    map.removeLayer(marker);
  }

  // Add new marker
  marker = L.marker(latLng, {
    title: "Current Position"
  }).bindPopup(`
    <div style="font-family: Arial; font-size: 12px;">
      <b>📍 Current Location</b><br>
      Latitude: ${gpsData.lat.toFixed(6)}°<br>
      Longitude: ${gpsData.lng.toFixed(6)}°<br>
      Altitude: ${gpsData.alt?.toFixed(2) || '--'} m<br>
      Satellites: ${gpsData.sat || '--'}<br>
      Accuracy: ${gpsData.hdop?.toFixed(1) || '--'}
    </div>
  `).addTo(map);

  marker.openPopup();

  // Add to location history
  locations.push(latLng);

  // Update polyline (track history)
  if (polyline) {
    map.removeLayer(polyline);
  }

  if (locations.length > 1) {
    polyline = L.polyline(locations, {
      color: 'blue',
      weight: 3,
      opacity: 0.7
    }).addTo(map);
  }

  // Center map on current position
  map.setView(latLng, 15);

  console.log(`📍 Map updated: ${gpsData.lat.toFixed(6)}, ${gpsData.lng.toFixed(6)}`);
}

function updateLastUpdate() {
  const now = new Date();
  const time = now.toLocaleTimeString('en-US', { 
    hour: '2-digit', 
    minute: '2-digit', 
    second: '2-digit',
    hour12: false 
  });
  lastUpdateEl.innerText = time;
}

// ================= INITIALIZE =================
console.log("Initializing GPS Tracker Dashboard...");
initMap();
console.log("✅ GPS Tracker Dashboard initialized");