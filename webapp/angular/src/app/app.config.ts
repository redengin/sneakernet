import { ApplicationConfig, provideZoneChangeDetection } from '@angular/core';
import { provideNoopAnimations } from '@angular/platform-browser/animations';

import { provideHttpClient } from '@angular/common/http';

import { provideRouter } from '@angular/router';
import { routes } from './app.routes';

// use @ng-icons to embed material icons
import { provideIcons, provideNgIconsConfig } from '@ng-icons/core';
import { matInfo, matCreateNewFolder } from '@ng-icons/material-icons/baseline';


export const appConfig: ApplicationConfig = {
  providers: [
    provideNoopAnimations(), // disable animation
    provideHttpClient(),

    provideIcons({ matInfo, matCreateNewFolder }),

    // provideZoneChangeDetection({ eventCoalescing: true }),
    // provideRouter(routes),
    // TODO provide input validation
    // { provide: ErrorStateMatcher, useClass: ShowOnDirtyErrorStateMatcher }
    // provideHttpClientTesting(),
   ]
};
