import { ApplicationConfig, provideZoneChangeDetection } from '@angular/core';
import { provideRouter } from '@angular/router';

// use @ng-icons to embed material icons
import { provideIcons, provideNgIconsConfig } from '@ng-icons/core';
import { matInfo, matCreateNewFolder } from '@ng-icons/material-icons/baseline';
import { matFolderOutline, matFileDownloadOutline, matDeleteOutline } from '@ng-icons/material-icons/outline';

// Change the title here
export const globals = {
  appTitle:"SneakerNet",
}

export const appConfig: ApplicationConfig = {
  providers: [
    provideIcons({
      matInfo, matCreateNewFolder,
      matFolderOutline, matFileDownloadOutline, matDeleteOutline
    }),
  ]
};
