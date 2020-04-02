# Introduction

An Electron application built with Angular for the Argonize Challenge for COVID-19

Currently runs with:

- Angular v9.0.2
- Electron v8.0.1
- Electron Builder v22.3.2

With this project, you can :

- Run your app in a local development environment with Electron & Hot reload
- Run your app in a production environment
- Package your app into an executable file for Linux, Windows & Mac

/!\ Hot reload only pertains to the renderer process. The main electron process is not able to be hot reloaded, only restarted.

/!\ Angular 9.x CLI needs Node 10.13 or later to works correctly.

## Dependencies
#### Node
https://nodejs.org/en/download/

#### Yarn
https://classic.yarnpkg.com/en/docs/install/#mac-stable

#### Angular CLI
``` bash
npm install -g @angular/cli
```
## Getting Started

Install dependencies with npm :

``` bash
yarn
```

## To build for development

- **in a terminal window** -> yarn start

## Included Commands

|Command|Description|
|--|--|
|`yarn ng:serve:web`| Execute the app in the browser |
|`yarn build`| Build the app. Your built files are in the /dist folder. |
|`yarn build:prod`| Build the app with Angular aot. Your built files are in the /dist folder. |
|`yarn electron:local`| Builds your application and start electron
|`yarn electron:linux`| Builds your application and creates an app consumable on linux system |
|`yarn electron:windows`| On a Windows OS, builds your application and creates an app consumable in windows 32/64 bit systems |
|`yarn electron:mac`|  On a MAC OS, builds your application and generates a `.app` file of your application that can be run on Mac |