import { Component, Inject } from '@angular/core';
import { HttpClient, HttpEventType, HttpParams } from '@angular/common/http';

import { KeyValuePipe, formatDate } from '@angular/common';

import { NgIconsModule } from '@ng-icons/core';

import { Toolbar } from './components/toolbar';

import { MatFormFieldModule } from '@angular/material/form-field'
import { MatInputModule } from '@angular/material/input'
import { MatIconModule } from '@angular/material/icon';

import { TimeagoModule } from 'ngx-timeago';
import { NgxFilesizeModule } from 'ngx-filesize';

import { MatDialog, MAT_DIALOG_DATA } from '@angular/material/dialog';
import { MatProgressSpinnerModule } from '@angular/material/progress-spinner';
import { MatProgressBarModule } from '@angular/material/progress-bar';

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
    const dialogRef = this.dialog.open(SpinnerDialog, { disableClose: true });
    this.folderData = {};
    this.http.get<Folder>(`api/catalog/${this.currentPath}/`)
      .subscribe({
        next: (data) => { this.folderData = data; },
        error: () => { dialogRef.close(); },
        complete: () => { dialogRef.close(); }
      });
  }

  chooseParentFolder(): void {
    const folders = this.currentPath.split('/');
    if (folders.length > 2)
      this.currentPath = folders.slice(0, -1).join('/');
    else this.currentPath = '';
    this.getFolderData();
  }

  chooseSubfolder(subfolder: string): void {
    this.currentPath += `/${subfolder}`;
    this.getFolderData();
  }

  createSubfolder(event: any): void {
    const subfolderName = event.target.value;
    event.target.value = null; /* clear the DOM value */
    const dialogRef = this.dialog.open(SpinnerDialog, { disableClose: true });
    const path = this.currentPath ? `${this.currentPath}/${subfolderName}` : subfolderName;
    this.http.put(`api/catalog/${path}/`, null)
      .subscribe({
        next: () => { this.currentPath = path },
        error: (error) => {
          dialogRef.close();
          // TODO alert user of failure
          console.error(`Failed to create subfolder ${error}`);
        },
        complete: () => {
          dialogRef.close();
          this.getFolderData();
        },
      });
  }

  removeFolder(path: string): void {
    const dialogRef = this.dialog.open(SpinnerDialog, { disableClose: true });
    this.http.delete(`api/catalog/${path}/`)
      .subscribe({
        next: () => this.chooseParentFolder(),
        error: (error) => {
          dialogRef.close();
          // TODO raise up error dialog
          console.error(error);
        },
        complete: () => {
          dialogRef.close();
        },
      })
  }

  deleteFile(filename: string): void {
    const dialogRef = this.dialog.open(SpinnerDialog, { disableClose: true });
    this.http.delete(`api/catalog/${this.currentPath}/${filename}`)
      .subscribe({
        next: () => this.getFolderData(),
        error: (error) => {
          dialogRef.close();
          // TODO raise up error dialog
          console.error(error);
        },
        complete: () => {
          dialogRef.close();
        },
      })
  }

  addFile(event: Event): void {
    this.closeChooseUploadDialog();
    // get the files from the event target
    const fileSelect = event.target as HTMLInputElement;
    if (fileSelect.files) {
      const fileList: FileList = fileSelect.files;
      for (const file of fileList) {
        const dialogRef = this.dialog.open(ProgressDialog,
          { disableClose: true,
            data: {
              'title' : `${file.name}`,
              'progress_pct': 0,
            }
          }
        );
        // const timestamp = new Date(file.lastModified).toISOString();
        this.http.put(`api/catalog/${this.currentPath}/${file.name}`,
          // body data
          file,
          // additional options
          {
            headers: {
              'X-timestamp':
                // use file timestamp: new Date(file.lastModified).toISOString(),
                // using current date for upload timestamp
                new Date().toISOString(),
            },
            reportProgress: true,
            observe: 'events'
          },
        )
        .subscribe( event => {
          switch (event.type) {
            case HttpEventType.UploadProgress:
              const progress_pct = event.total ? (100 * (event.loaded / event.total)) : 0;
              dialogRef.componentInstance.data.progress_pct = progress_pct;
              console.log(`progress ${progress_pct}%`);
              break;
            case HttpEventType.ResponseHeader:
            case HttpEventType.Response:
              // TODO handle error
              dialogRef.close();
              this.getFolderData();
              break;
          }
        });
      }
    }
  }

  dialogRef: any;
  openChooseUploadDialog() {
    this.dialogRef = this.dialog.open(ChooseUploadDialog);
  }
  closeChooseUploadDialog() {
    this.dialogRef.close();
  }
}


import { ClipboardModule } from '@angular/cdk/clipboard';
@Component({
  templateUrl: './choose_upload_dialog.html',
  imports: [ClipboardModule]
})
export class ChooseUploadDialog {
};

@Component({
  template: '<mat-spinner style="margin:10px auto;">',
  imports: [MatProgressSpinnerModule],
})
export class SpinnerDialog {
};

@Component({
  templateUrl: './progress_dialog.html',
  imports: [MatProgressBarModule, MatProgressSpinnerModule],
})
export class ProgressDialog {
  constructor(@Inject(MAT_DIALOG_DATA) public data: any) {}
};