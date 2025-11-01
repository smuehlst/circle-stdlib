<script lang="ts">
  import { onMount, onDestroy, tick } from 'svelte';
  import { writable } from 'svelte/store';

  // Store to hold the messages
  const messages = writable<string[]>([]);
  let ws: WebSocket | null = null;
  let url = '';

  // UI state
  let input = '';
  let connected = false;
  let messagesContainer: HTMLUListElement | null = null;

  function sendMessage() {
    const text = input.trim();
    if (!text) return;

    if (ws && ws.readyState === WebSocket.OPEN) {
      ws.send(text);
      // Show outgoing message in the chat
      messages.update((m) => [...m, `You: ${text}`]);
      input = '';
    } else {
      // Optionally queue or show an error
      messages.update((m) => [...m, `System: Unable to send, not connected`]);
    }
  }

  onMount(() => {
    // Compute the websocket url on the client so it matches the page origin
    const wsProto = location.protocol === 'https:' ? 'wss:' : 'ws:';
    // use location.host (hostname:port) so it connects to the same server the page came from
    url = `${wsProto}//${location.host}/ws`;

    // 1. Establish the connection
    ws = new WebSocket(url);

    // 2. Handle incoming messages
    ws.onmessage = (event) => {
      messages.update((m) => [...m, `Server: ${event.data}`]);
    };

    // 3. Handle connection opening
    ws.onopen = () => {
      console.log('WebSocket connection established!');
      connected = true;
      ws?.send("Hello from SvelteKit Client!");
    };

    // 4. Handle errors and closing
    ws.onerror = (error) => {
      console.error('WebSocket Error:', error);
      messages.update((m) => [...m, 'System: WebSocket error']);
    };
    ws.onclose = () => {
      console.log('WebSocket connection closed.');
      connected = false;
      messages.update((m) => [...m, 'System: connection closed']);
    };

    // auto-scroll when messages change
    const unsubscribe = messages.subscribe(async () => {
      await tick();
      if (messagesContainer) {
        messagesContainer.scrollTo({ top: messagesContainer.scrollHeight });
      }
    });

    return () => {
      unsubscribe();
    };
  });

  onDestroy(() => {
    // 5. Clean up when the component is destroyed
    if (ws) {
      ws.close();
    }
  });
</script>

<div class="max-w-2xl mx-auto">
  <h1 class="text-2xl font-semibold mb-4">WebSocket Chat</h1>

  <div class="border rounded bg-white shadow-sm">
    <!-- Messages area -->
    <ul bind:this={messagesContainer} class="h-64 overflow-auto p-4 space-y-2">
      {#each $messages as message}
        <li class="text-sm text-gray-800">{message}</li>
      {/each}
    </ul>

    <!-- Input area -->
    <form class="flex border-t p-3 gap-2" on:submit|preventDefault={sendMessage}>
      <input
        class="flex-1 border rounded px-3 py-2 text-sm"
        placeholder={connected ? 'Type a message...' : 'Connecting...'}
        bind:value={input}
        on:keydown={(e) => e.key === 'Enter' && sendMessage()}
      />
      <button
        type="submit"
        class="bg-indigo-600 text-white px-4 py-2 rounded text-sm disabled:opacity-50"
        disabled={!connected}
      >
        Send
      </button>
    </form>
  </div>
</div>