const path = require('path');

const esbuild = require('esbuild');

const GLOBAL_CONFIG = require('../some-config.json');

const {
  ensureDirectoryExists,
  ringBell
} = require('./platform.js');

const P = (p) => path.resolve(__dirname, p);

function esBuildConfig() {
  console.log('Global Config', GLOBAL_CONFIG);

  return {
    entryPoints: [P("../src/foo.ts")],
    bundle: true,
    minify: false,  // TODO: turn on minification!
    treeShaking: true,
    outdir: P("../some/path"),
    define: GLOBAL_CONFIG
  };
}

function esBuildConfigDev() {
  return Object.assign(
    esBuildConfig(),
    {
      minify: false,
      watch: {
        onRebuild(error, result) {
          // TODO: throttle the rebuild event, otherwise branch changes cause a ton of sequential re-builds
          if (error){
            ringBell();
            console.error(error);
          } else {
            if (result.warnings.length) {
              console.log('Warnings:');
              logResults(result.warnings);
            } else if (result.errors.length) {
              console.log('Errors:');
              logResults(result.errors);
            }
            console.log('\nBuild succeeded');
            console.log(`    ${new Date().toString()}\n`);
          }
        },
      },
    }
  );
}

function esBuildConfigDevNoWatch() {
  return Object.assign(
    esBuildConfig(),
    {
      minify: false,
    }
  );
}

async function build(dev) {
  const config = dev ? esBuildConfigDev() : esBuildConfig();

  console.log(`JS build: ${dev ? 'dev' : 'prod'}`);

  ensureDirectoryExists(P('../dist'));

  return esbuild
    .build(config)
    .catch(() => {
      process.exit(1)
    });
}

/* TODO: Replace build() with this function once after testing dev-builder script
 * also some other stuff
*/
async function buildNoCatch(dev) {
  const config = dev ? esBuildConfigDevNoWatch() : esBuildConfig();

  console.log(`JS build: ${dev ? 'dev' : 'prod'}`);

  ensureDirectoryExists(P('../dist'));

  return esbuild.build(config);
}

function logResults(results) {
  for (const result of results) {
    console.log(result);
  }
}

module.exports = {
  build,
  buildNoCatch,
  logResults
};
