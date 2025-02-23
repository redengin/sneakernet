import { globals } from '../app.config';
import { Component } from '@angular/core';
import { NgIcon } from '@ng-icons/core';

import { inject } from '@angular/core';
import { MatDialog, MatDialogRef } from '@angular/material/dialog';

@Component({
  selector: 'app-toolbar',
  imports: [NgIcon],
  templateUrl: './toolbar.html',
})
export class Toolbar {
  readonly title = globals.appTitle;

  readonly dialog = inject(MatDialog);
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

  readonly dialogRef = inject(MatDialogRef<AboutDialog>);
  onNoClick(): void {
    this.dialogRef.close();
  }
};