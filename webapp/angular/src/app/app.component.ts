import { Component } from '@angular/core';
import { RouterOutlet } from '@angular/router';

import { MatToolbarModule } from '@angular/material/toolbar';
import { MatButtonModule } from '@angular/material/button';
import { NgIcon } from '@ng-icons/core';
import { inject, signal, model } from '@angular/core';
import { MatDialog, MatDialogRef, MatDialogTitle, MatDialogContent, MatDialogActions, MatDialogClose, MAT_DIALOG_DATA } from '@angular/material/dialog';
import { MatInputModule } from '@angular/material/input';
import { MatSelectModule } from '@angular/material/select';
import { FormsModule } from '@angular/forms';
import { MatFormField, MatFormFieldModule, MatLabel } from '@angular/material/form-field';

export var title = "Sneakernet"


@Component({
  selector: 'app-toolbar',
  templateUrl: './toolbar.html',
  imports: [MatToolbarModule, MatButtonModule, NgIcon],
})
export class AppToolbar {
  readonly title = title;

  readonly dialog = inject(MatDialog);
  openDialog(): void {
    this.dialog.open(AboutDialog);
  }
};
@Component({
  templateUrl: './about_dialog.html',
  imports: [MatDialogTitle, MatDialogContent],
})
export class AboutDialog {
  readonly title = title;
  readonly dialogRef = inject(MatDialogRef<AboutDialog>);
  onNoClick(): void {
    this.dialogRef.close();
  }
};



type PathData = {
  locked?: boolean;
  subfolders?: string[];
  files?: Content[];
};
type Content = {
  name?: string;
  size?: BigInteger;
  timestamp?: string;
  title?: string;
  hasIcon?: boolean;
}

@Component({
  selector: 'app-root',
  templateUrl: './app.component.html',
  styleUrl: './app.component.css',
  imports: [
    RouterOutlet,
    AppToolbar,
    MatInputModule, MatSelectModule, MatFormFieldModule,
    MatButtonModule, NgIcon,
  ],
})
export class AppComponent {

  currentPath = "/";

  pathData: PathData = {
    // locked: true,
    // subfolders: ['hello', 'world'],
  };

  readonly dialog = inject(MatDialog)
// New Folder handler
  // readonly newfolder = signal('');
  readonly newfolder = signal('');
  openNewFolderDialog(): void {
    const dialogRef = this.dialog.open(NewFolderDialog, {
      // data: {name: this.name(), animal: this.animal()},
      data: {
        currentPath: this.currentPath,
        newfolder: this.newfolder(),
      }
    });

    dialogRef.afterClosed().subscribe(result => {
      console.log(`The dialog was closed result:${result}`);
      // if (result !== undefined) {
      //   this.newfolder.set(result);
      // }
    });
  }

}
export interface NewFolderDialogData {
  currentPath: string;
  newfolder: string;
}
@Component({
  templateUrl: './new_folder_dialog.html',
  imports: [
    MatDialogTitle, MatDialogTitle, MatDialogContent, MatDialogActions, MatDialogClose,
    FormsModule, MatFormField, MatLabel, MatInputModule,
  ],
})
export class NewFolderDialog {
  readonly dialogRef = inject(MatDialogRef<AboutDialog>);
  readonly data = inject<NewFolderDialogData>(MAT_DIALOG_DATA);
  readonly newfolder = model(this.data.newfolder);
  onNoClick(): void {
    this.dialogRef.close();
  }
};
