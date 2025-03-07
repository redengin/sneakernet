import { globals } from '../app.config';
import { Component } from '@angular/core';
import { NgIconsModule } from '@ng-icons/core';

import { MatDialog, MatDialogRef } from '@angular/material/dialog';

@Component({
  selector: 'app-toolbar',
  imports: [NgIconsModule],
  templateUrl: './toolbar.html',
})
export class Toolbar {
  constructor(private dialog: MatDialog) { }

  readonly title = globals.appTitle;

  openDialog(): void {
    this.dialog.open(AboutDialog);
  }
}

@Component({
  templateUrl: './about_dialog.html',
  // save 5KB by not using mat-dialog-* components
})
export class AboutDialog {
  readonly title = globals.appTitle;
};