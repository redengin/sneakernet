import { Component } from '@angular/core';
import { HttpClient, HttpParams } from '@angular/common/http';

import { KeyValuePipe, formatDate } from '@angular/common';

import { NgIconsModule } from '@ng-icons/core';

import { Toolbar } from './components/toolbar';

import { MatDialog, MatDialogRef } from '@angular/material/dialog';
import { ProgressDialog } from './components/progress_dialog';

import { MatFormFieldModule } from '@angular/material/form-field'
import { MatInputModule } from '@angular/material/input'
import { MatIconModule } from '@angular/material/icon';

import { TimeagoModule } from 'ngx-timeago';
import { NgxFilesizeModule } from 'ngx-filesize';

// Types per openapi/catalog.yml
//==============================================================================
type Entry = {
  isFolder: boolean;
  // following only for file entries (i.e. isFolder = false)
  size?: number;
  timestamp?: string;
  title?: string;
  hasIcon?: boolean;
}

type Folder = {
  readonly locked?: boolean;
  readonly entries?: {
    [key: string]: Entry;
  }
};

@Component({
  selector: 'app-root',
  templateUrl: './app.component.html',
  styleUrl: './app.component.css',
  imports: [
    Toolbar,
    NgIconsModule, KeyValuePipe,
    MatFormFieldModule, MatInputModule, MatIconModule,
    TimeagoModule, NgxFilesizeModule,
  ],
})
export class AppComponent {
  constructor(private http: HttpClient, private dialog: MatDialog) { }
  ngOnInit() { this.getFolderData(); }

  currentPath = '';
  folderData: Folder = {};

  getFolderData(): void {
    const dialogRef = this.openLoadingDialog();
    this.folderData = {};
    this.http.get<Folder>(`api/catalog/${this.currentPath}/`)
      .subscribe(body => { this.folderData = body; dialogRef.close(); });
  }

  chooseParentFolder(): void {
    const folders = this.currentPath.split('/');
    if (folders.length > 2)
      this.currentPath = folders.slice(0, -1).join('/');
    else this.currentPath = '';
    this.getFolderData();
  }

  chooseSubfolder(subfolder: string): void {
    this.currentPath += `${subfolder}`;
    this.getFolderData();
  }

  createSubfolder(event: any): void {
    const subfolderName = event.target.value;
    event.target.value = null; /* clear the DOM value */
    const dialogRef = this.openLoadingDialog();
    const path = this.currentPath ? `${this.currentPath}/${subfolderName}` : subfolderName;
    this.http.put(`api/catalog/${path}/`, null)
      .subscribe({
        complete: () => {
          dialogRef.close();
          this.currentPath = path;
          this.getFolderData();
        },
        error: (error) => {
          dialogRef.close();
          // TODO alert user of failure
          console.error(`Failed to create subfolder ${error}`);
        }
      });
  }

  removeFolder(path: string): void {
    const dialogRef = this.openLoadingDialog();
    this.http.delete(`api/catalog/${path}/`)
      .subscribe({
        complete: () => {
          dialogRef.close();
          this.chooseParentFolder()
        },
        error: (error) => {
          dialogRef.close();
          // TODO raise up error dialog
          console.error(error);
        }
      })
  }

  deleteFile(filename: string): void {
    const dialogRef = this.openLoadingDialog();
    // const path = this.currentPath ? `${this.currentPath}/${filename}` : filename;
    this.http.delete(`api/catalog/${this.currentPath}/${filename}`)
      .subscribe({
        complete: () => {
          dialogRef.close();
          this.getFolderData()
        },
        error: (error) => {
          dialogRef.close();
          // TODO raise up error dialog
          console.error(error);
        }
      })
  }

  addFile(event: Event): void {
    this.closeChooseUploadDialog();
    // get the files from the event target
    const fileSelect = event.target as HTMLInputElement;
    if (fileSelect.files) {
      const fileList: FileList = fileSelect.files;
      for (const file of fileList) {
        const dialogRef = this.openLoadingDialog();
        // const timestamp = new Date(file.lastModified).toISOString();
        this.http.put(`api/catalog/${this.currentPath}/${file.name}`,
          // data
          file,
          // additional options
          {
            headers: {
              'X-timestamp':
                // use file timestamp new Date(file.lastModified).toISOString(),
                // using now() for timestamp
                new Date().toISOString(),
            }
          }
        ).subscribe({
          complete: () => {
            dialogRef.close();
            this.getFolderData()
          },
          error: (error) => {
            dialogRef.close();
            // TODO raise up error dialog
            console.error(error);
          }
        });
      }
    }
  }

  openLoadingDialog() {
    return this.dialog.open(ProgressDialog, { disableClose: true });
  }

  dialogRef: any;
  openChooseUploadDialog() {
    this.dialogRef = this.dialog.open(ChooseUploadDialog);
  }
  closeChooseUploadDialog() {
    this.dialogRef.close();
  }
}

@Component({
  templateUrl: './choose_upload_dialog.html',
  // save 5KB by not using mat-dialog-* components
})
export class ChooseUploadDialog {
};