import { globals } from './app.config';
import { Component } from '@angular/core';
import { NgIcon } from '@ng-icons/core';

// import { MatDialog, MAT_DIALOG_DATA, MatDialogRef,
//          MatDialogTitle, MatDialogContent,
//          MatDialogActions, MatDialogClose
// } from '@angular/material/dialog';

@Component({
  selector: 'app-toolbar',
  imports: [ NgIcon ],
  templateUrl: './toolbar.html',
})
export class Toolbar {
    title = globals.appTitle;

    openDialog() : void 
    {

    }
}