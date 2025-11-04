import tailwindcss from '@tailwindcss/vite';
import { sveltekit } from '@sveltejs/kit/vite';
import { defineConfig } from 'vite';

export default defineConfig({
  plugins: [
    tailwindcss(),
    sveltekit(),
    {
      name: 'mock-server',
      /**
       * @param {any} server
       */
      configureServer(server) {
        /**
         * @param {any} req
         * @param {any} res
         * @param {Function} next
         */
        server.middlewares.use('/api/status', (req, res, next) => {
          if ((req as any).method === 'GET') {
            res.setHeader('Content-Type', 'application/json');
            res.end(JSON.stringify({"cpuSpeed":1400000000,"gpioFanPin":0,"socMaxTemp":60}));
          } else {
            next();
          }
        });
      }
    }
  ]
});
