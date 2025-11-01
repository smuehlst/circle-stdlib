<script lang="ts">
    import { onMount } from "svelte";

    let cpuSpeed = 0;
    let gpioFanPin = 0;
    let socMaxTemp = 0;

    onMount(async () => {
        const response = await fetch("/api/status");
        if (response.ok) {
            const data = await response.json();
            cpuSpeed = data.cpuSpeed;
            gpioFanPin = data.gpioFanPin;
            socMaxTemp = data.socMaxTemp;
        } else {
            console.error("Failed to fetch status");
        }
    });
</script>

<h1>System Status Fetched Via REST API</h1>
<ul>
    <li>CPU Speed: {cpuSpeed}</li>
    <li>GPIO Fan Pin: {gpioFanPin}</li>
    <li>SoC Max Temperature: {socMaxTemp} °C</li>
</ul>
