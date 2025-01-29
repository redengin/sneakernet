import { ApplicationConfig, provideZoneChangeDetection } from '@angular/core';
import { provideRouter } from '@angular/router';

import { routes } from './app.routes';

// FIXME do we want material animations?
// import { provideAnimationsAsync } from '@angular/platform-browser/animations/async';

// TODO use @ng-icons to embed material icons
import { provideIcons, provideNgIconsConfig } from '@ng-icons/core';
import { matInfo, matCreateNewFolder } from '@ng-icons/material-icons/baseline';


export const appConfig: ApplicationConfig = {
  providers: [
    provideZoneChangeDetection({ eventCoalescing: true }),
    provideRouter(routes),
    provideIcons({ matInfo, matCreateNewFolder }),
  ]
};
