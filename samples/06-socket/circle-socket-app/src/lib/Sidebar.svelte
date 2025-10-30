<script lang="ts">
    // A small nav structure. Expand or replace with dynamic data as needed.
    type NavChild = { title: string; href: string };
    type NavItem = { title: string; href?: string; children?: NavChild[] };

    const nav: NavItem[] = [
        { title: 'Overview', href: '/' },
        {
            title: 'Repositories',
            children: [
                { title: 'List repositories', href: '/repos' },
                { title: 'Create repository', href: '/repos/new' }
            ]
        },
        {
            title: 'Issues',
            children: [
                { title: 'List issues', href: '/issues' },
                { title: 'Create issue', href: '/issues/new' }
            ]
        },
        { title: 'Users', href: '/users' }
    ];

    // Track open menus by index. We use a Set and reassign to trigger reactivity.
    let open = new Set<number>();

    function toggle(i: number) {
        if (open.has(i)) {
            open.delete(i);
        } else {
            // close other menus if you want accordion-like behavior
            // open.clear();
            open.add(i);
        }
        open = new Set(open);
    }

    // Mobile state
    let mobileOpen = false;
    function closeMobile() {
        mobileOpen = false;
    }
</script>

<nav class="sidebar hidden md:block md:w-64 fixed left-0 top-0 h-full bg-white border-r border-gray-200 overflow-auto">
    <div class="px-4 py-6">
        <h2 class="text-sm font-semibold text-gray-600 uppercase tracking-wide mb-4">Documentation</h2>

        <ul class="space-y-1">
            {#each nav as item, i}
                <li>
                    {#if item.children}
                        <div>
                            <button
                                aria-expanded={open.has(i)}
                                aria-controls={`submenu-${i}`}
                                on:click={() => toggle(i)}
                                class="w-full flex items-center justify-between px-2 py-2 rounded text-left text-gray-800 hover:bg-gray-50 focus:outline-none focus:ring-2 focus:ring-indigo-500"
                            >
                                <span class="font-medium">{item.title}</span>
                                <!-- Arrow SVG: points down when closed, up when open -->
                                <svg
                                    class="ml-2 h-4 w-4 transition-transform duration-150"
                                    viewBox="0 0 20 20"
                                    fill="none"
                                    xmlns="http://www.w3.org/2000/svg"
                                    aria-hidden="true"
                                    class:rotate-180={open.has(i)}
                                >
                                    <path d="M5 8l5 5 5-5" stroke="currentColor" stroke-width="1.5" stroke-linecap="round" stroke-linejoin="round" />
                                </svg>
                            </button>

                            {#if open.has(i)}
                                <ul id={`submenu-${i}`} class="mt-1 pl-4 border-l border-gray-100">
                                    {#each item.children as child}
                                        <li>
                                            <a href={child.href} class="block px-2 py-2 text-sm text-gray-700 hover:bg-gray-50 rounded">{child.title}</a>
                                        </li>
                                    {/each}
                                </ul>
                            {/if}
                        </div>
                    {:else}
                        <a href={item.href} class="block px-2 py-2 rounded text-gray-800 hover:bg-gray-50">{item.title}</a>
                    {/if}
                </li>
            {/each}
        </ul>
    </div>
</nav>

<!-- Mobile top bar + overlay drawer -->

<!-- Mobile header - visible only on small screens -->
<div class="md:hidden fixed top-0 left-0 right-0 bg-white border-b z-40 flex items-center justify-between px-4 h-12">
    <button
        class="p-2 rounded focus:outline-none focus:ring-2 focus:ring-indigo-500"
        aria-label="Toggle navigation"
        on:click={() => (mobileOpen = !mobileOpen)}
    >
        <!-- Hamburger -->
        <svg class="w-6 h-6" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round" aria-hidden="true">
            <path d="M3 12h18M3 6h18M3 18h18" />
        </svg>
    </button>

    <div class="font-semibold text-sm">Documentation</div>

    <div style="width:32px;" aria-hidden="true"></div>
</div>

{#if mobileOpen}
    <!-- Overlay drawer: nav on the left and a transparent backdrop on the right -->
    <div class="fixed inset-0 z-50 flex">
        <div class="w-64 bg-white border-r border-gray-200 overflow-auto">
            <div class="px-4 py-4 flex items-center justify-between border-b">
                <div class="font-semibold text-sm">Documentation</div>
                <button class="p-2 rounded focus:outline-none focus:ring-2 focus:ring-indigo-500" aria-label="Close navigation" on:click={closeMobile}>
                    <svg class="w-5 h-5" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round" aria-hidden="true">
                        <path d="M6 18L18 6M6 6l12 12" />
                    </svg>
                </button>
            </div>

            <div class="px-4 py-6">
                <ul class="space-y-1">
                    {#each nav as item, i}
                        <li>
                            {#if item.children}
                                <div>
                                    <button
                                        aria-expanded={open.has(i)}
                                        aria-controls={`mobile-submenu-${i}`}
                                        on:click={() => toggle(i)}
                                        class="w-full flex items-center justify-between px-2 py-2 rounded text-left text-gray-800 hover:bg-gray-50 focus:outline-none focus:ring-2 focus:ring-indigo-500"
                                    >
                                        <span class="font-medium">{item.title}</span>
                                        <svg
                                            class="ml-2 h-4 w-4 transition-transform duration-150"
                                            viewBox="0 0 20 20"
                                            fill="none"
                                            xmlns="http://www.w3.org/2000/svg"
                                            aria-hidden="true"
                                            class:rotate-180={open.has(i)}
                                        >
                                            <path d="M5 8l5 5 5-5" stroke="currentColor" stroke-width="1.5" stroke-linecap="round" stroke-linejoin="round" />
                                        </svg>
                                    </button>

                                    {#if open.has(i)}
                                        <ul id={`mobile-submenu-${i}`} class="mt-1 pl-4 border-l border-gray-100">
                                            {#each item.children as child}
                                                <li>
                                                    <a href={child.href} on:click={closeMobile} class="block px-2 py-2 text-sm text-gray-700 hover:bg-gray-50 rounded">{child.title}</a>
                                                </li>
                                            {/each}
                                        </ul>
                                    {/if}
                                </div>
                            {:else}
                                <a href={item.href} on:click={closeMobile} class="block px-2 py-2 rounded text-gray-800 hover:bg-gray-50">{item.title}</a>
                            {/if}
                        </li>
                    {/each}
                </ul>
            </div>
        </div>

        <!-- Backdrop: clicking closes the drawer -->
        <div
            class="flex-1 bg-black/40"
            role="button"
            tabindex="0"
            on:click={closeMobile}
            on:keydown={(e) => (e.key === 'Enter' || e.key === ' ') && closeMobile()}
        ></div>
    </div>
{/if}

<style>
    /* The rotate-180 class is provided by Tailwind if configured; add a fallback for environments without it */
    .rotate-180 {
        transform: rotate(180deg);
    }
</style>
