import adapter from '@sveltejs/adapter-static';
import { vitePreprocess } from '@sveltejs/vite-plugin-svelte';

/** @type {import('@sveltejs/kit').Config} */
const config = {
	// Consult https://svelte.dev/docs/kit/integrations
	// for more information about preprocessors
	preprocess: vitePreprocess({
        // Pass compiler options to the Svelte preprocessor
        compilerOptions: {
            // This forces runes mode project-wide and disables legacy syntax 
            // for all *your application's* Svelte components.
            runes: true 
        }
    }),
	kit: { adapter: adapter() }
};

export default config;
