<script lang="ts">
	import '../app.css';
	import favicon from '$lib/assets/favicon.svg';
	import Sidebar from '$lib/Sidebar.svelte';

	let { children } = $props();
</script>

<svelte:head>
	<link rel="icon" href={favicon} />
</svelte:head>

<!-- Layout with persistent left sidebar and a main content area. -->
<div class="min-h-screen flex bg-gray-50">
	<Sidebar />

	<main class="flex-1 ml-0 md:ml-64 p-6">
		{@render children()}
	</main>
</div>
