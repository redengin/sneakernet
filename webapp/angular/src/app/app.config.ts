import { ApplicationConfig, provideZoneChangeDetection } from '@angular/core';
import { provideRouter } from '@angular/router';
import { provideNoopAnimations } from '@angular/platform-browser/animations';

import { routes } from './app.routes';

// use @ng-icons to embed material icons
import { provideIcons, provideNgIconsConfig } from '@ng-icons/core';
import { matInfo, matCreateNewFolder } from '@ng-icons/material-icons/baseline';


export const appConfig: ApplicationConfig = {
  providers: [
    provideNoopAnimations(), // disable animation
    provideZoneChangeDetection({ eventCoalescing: true }),
    provideRouter(routes),
    provideIcons({ matInfo, matCreateNewFolder }),
    // TODO provide input validation
    // { provide: ErrorStateMatcher, useClass: ShowOnDirtyErrorStateMatcher }
  ]
};
