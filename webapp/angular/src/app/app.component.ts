import { Component } from '@angular/core';
import { RouterOutlet } from '@angular/router';

import { MatToolbarModule } from '@angular/material/toolbar';
import { MatButtonModule } from '@angular/material/button';
import { NgIcon } from '@ng-icons/core';
import { inject } from '@angular/core';
import { MatDialog, MatDialogRef, MatDialogTitle, MatDialogContent } from '@angular/material/dialog';
import { MatInputModule } from '@angular/material/input';
import { MatSelectModule } from '@angular/material/select';
import { MatFormFieldModule } from '@angular/material/form-field';

export var title = "Sneakernet"


@Component({
  selector: 'app-toolbar',
  templateUrl: './toolbar.html',
  imports: [MatToolbarModule, MatButtonModule, NgIcon],
})
export class AppToolbar{
  readonly title = title;

  readonly dialog = inject(MatDialog);
  openDialog(): void {
    this.dialog.open(AboutDialog);
  }
};
@Component({
  selector: 'about-dialog',
  templateUrl: './about_dialog.html',
  imports: [MatDialogTitle, MatDialogContent],
})
export class AboutDialog{
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

  pathData : PathData = {
    // locked: true,
    // subfolders: ['hello', 'world'],
  };

  createSubfolder() : void
  {

  }
}
