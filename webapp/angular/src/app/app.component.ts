import { Component } from '@angular/core';
import { HttpClient } from '@angular/common/http';
import { retry } from 'rxjs';

import { RouterOutlet } from '@angular/router';

import { MatToolbarModule } from '@angular/material/toolbar';
import { MatButtonModule } from '@angular/material/button';
import { NgIconsModule } from '@ng-icons/core';
import { inject, signal, model } from '@angular/core';
import { MatDialog, MAT_DIALOG_DATA, MatDialogRef, MatDialogTitle, MatDialogContent, MatDialogActions, MatDialogClose } from '@angular/material/dialog';
import { MatInputModule } from '@angular/material/input';
import { MatSelectModule } from '@angular/material/select';
import { FormsModule } from '@angular/forms';
import { MatFormField, MatFormFieldModule, MatLabel } from '@angular/material/form-field';

export var title = "Sneakernet"

// Toolbar
//==============================================================================
@Component({
  selector: 'app-toolbar',
  templateUrl: './toolbar.html',
  imports: [MatToolbarModule, MatButtonModule, NgIconsModule],
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
//==============================================================================


// Sneakernet Types
//==============================================================================
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
//==============================================================================

// App Root
//==============================================================================
@Component({
  selector: 'app-root',
  templateUrl: './app.component.html',
  styleUrl: './app.component.css',
  imports: [
    RouterOutlet,
    AppToolbar,
    MatInputModule, MatSelectModule, MatFormFieldModule,
    MatButtonModule, NgIconsModule,
  ],
})
export class AppComponent {
  currentPath = "/";
  pathData: PathData = {};

  constructor(private http: HttpClient) { }
  ngOnInit()
  {
    // this.http.get<PathData>('http://localhost:3000/catalog/')
    this.http.get<PathData>('api/catalog')
      .pipe( retry({ delay: 500 /* ms */ }))
      .subscribe(body => this.pathData = body);
  }



  // provide dialogs
  readonly dialog = inject(MatDialog);

// New Folder handler
//------------------------------------------------------------------------------
  newfolder = "";
  openNewFolderDialog(): void {
    const dialogRef = this.dialog.open(NewFolderDialog, {
      data: {
        currentPath: this.currentPath,
        newfolder: this.newfolder,
      }
    });

    dialogRef.afterClosed().subscribe(result => {
      console.log(`The dialog was closed result:${result}`);
      // if (result !== undefined) {
      //   this.newfolder.set(result);
      // }
    });
  }
//------------------------------------------------------------------------------

}
//==============================================================================

// New Folder Dialog
//==============================================================================
export interface NewFolderDialogData {
  currentPath: string;
  newfolder: string;
}
@Component({
  templateUrl: './new_folder_dialog.html',
  imports: [
    MatDialogTitle, MatDialogContent,
    FormsModule, MatFormField, MatInputModule,
    MatDialogActions, MatDialogClose,
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
//==============================================================================
