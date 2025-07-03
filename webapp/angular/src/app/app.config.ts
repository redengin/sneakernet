import { ApplicationConfig, importProvidersFrom, provideZoneChangeDetection } from '@angular/core';

import { provideHttpClient } from '@angular/common/http';

// use @ng-icons to embed material icons
import { provideIcons, provideNgIconsConfig } from '@ng-icons/core';
import { matInfo, matUploadFile } from '@ng-icons/material-icons/baseline';
import { matCreateNewFolderOutline, matFolderDeleteOutline,
         matFolderOutline, matFileDownloadOutline, matDeleteOutline } from '@ng-icons/material-icons/outline';

import { TimeagoModule } from 'ngx-timeago';

import { BackButtonDisableModule } from 'angular-disable-browser-back-button';

// Change the title here
export const globals = {
  appTitle:"SneakerNet",
}

export const appConfig: ApplicationConfig = {
  providers: [
    provideHttpClient(),
    provideIcons({
      matInfo,
      matCreateNewFolderOutline, matFolderDeleteOutline,
      matFolderOutline, matFileDownloadOutline, matDeleteOutline,
      matUploadFile
    }),
    importProvidersFrom(TimeagoModule.forRoot(), BackButtonDisableModule.forRoot()),
  ],
};
