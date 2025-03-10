import { Component } from '@angular/core';
import { HttpClient, HttpEventType, HttpParams } from '@angular/common/http';

import { KeyValuePipe, formatDate } from '@angular/common';

import { NgIconsModule } from '@ng-icons/core';

import { Toolbar } from './components/toolbar';

import { MatFormFieldModule } from '@angular/material/form-field'
import { MatInputModule } from '@angular/material/input'
import { MatIconModule } from '@angular/material/icon';

import { TimeagoModule } from 'ngx-timeago';
import { NgxFilesizeModule } from 'ngx-filesize';

import { MatDialog, MatDialogRef } from '@angular/material/dialog';
import { MatProgressSpinnerModule } from '@angular/material/progress-spinner';

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
        const dialogRef = this.dialog.open(SpinnerDialog, { disableClose: true });
        // const timestamp = new Date(file.lastModified).toISOString();
        const request = this.http.put(`api/catalog/${this.currentPath}/${file.name}`,
          // data
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
        );
        // FIXME provide progress
        // request.subscribe(
        //   event => {
        //     if (event.type == HttpEventType.UploadProgress) {
        //       if (event.total) {
        //         const progress = Math.round(100 * event.loaded / event.total);
        //         console.log(`upload ${file.name} ${progress}%`);
        //       }
        //     }
        //   },
        // );
        request.subscribe({
          error: (error) => {
            // TODO raise up error dialog
            console.error(`UHT OHT ${error}`);
          },
          complete: () => {
            dialogRef.close();
          },
        });
      }
      // refresh the folder data with the new files
      // FIXME this doesn't cause a refresh
      this.getFolderData();
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

@Component({
  templateUrl: './choose_upload_dialog.html',
})
export class ChooseUploadDialog {
};

@Component({
  template: '<mat-spinner style="margin:10px auto;">',
  imports: [MatProgressSpinnerModule],
})
export class SpinnerDialog {
};