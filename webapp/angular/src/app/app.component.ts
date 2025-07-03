import { Component, Inject } from '@angular/core';
import { HttpClient, HttpEventType, HttpParams } from '@angular/common/http';

import { KeyValuePipe, DecimalPipe, formatDate } from '@angular/common';

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
  hasIcon?: boolean;
  // following only for file entries (i.e. isFolder = false)
  size?: number;
  timestamp?: string;
  title?: string;
}

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

  currentPath: string = '';
  folderData: Map<string, Entry> = new Map(); 

  getFolderData(): void {
    const dialogRef = this.dialog.open(SpinnerDialog, { disableClose: true });
    this.http.get<Map<string,Entry>>(`api/catalog/${this.currentPath}/`)
      .subscribe({
        next: (data) => { this.folderData = data; },
        error: (response) => { dialogRef.close(); alert('Failed to retrieve catalog'); },
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
    event.target.blur();
    const subfolderName = event.target.value;
    event.target.value = null; /* clear the DOM value */
    const dialogRef = this.dialog.open(SpinnerDialog, { disableClose: true });
    const path = this.currentPath ? `${this.currentPath}/${subfolderName}` : subfolderName;
    this.http.put(`api/catalog/${path}/`, null)
      .subscribe({
        next: () => { this.currentPath = path },
        error: (response) => { dialogRef.close(); alert('Failed to create folder'); },
        complete: () => {
          dialogRef.close();
          this.currentPath = path;
          this.getFolderData();
        },
      });
  }

  removeFolder(path: string): void {
    const dialogRef = this.dialog.open(SpinnerDialog, { disableClose: true });
    this.http.delete(`api/catalog/${path}/`)
      .subscribe({
        next: () => this.chooseParentFolder(),
        error: (response) => { dialogRef.close(); alert('Failed to remove folder'); },
        complete: () => {
          dialogRef.close();
        },
      })
  }

  httpDecode(s: string): string {
    return decodeURIComponent(s);
  }

  deleteFile(filename: string): void {
    const dialogRef = this.dialog.open(SpinnerDialog, { disableClose: true });
    this.http.delete(`api/catalog/${this.currentPath}/${filename}`)
      .subscribe({
        next: () => this.getFolderData(),
        error: (response) => { dialogRef.close(); alert('Failed to delete file'); },
        complete: () => {
          dialogRef.close();
        },
      })
  }

  addFile(event: Event): void {
    // clear the Choose dialog
    this.dialogRef.close();
    // get the files from the event target
    const fileSelect = event.target as HTMLInputElement;
    if (fileSelect.files) {
      const fileList: FileList = fileSelect.files;
      for (const file of fileList) {
        this.dialogRef = this.dialog.open(ProgressDialog,
          {
            data: {
              'title': `${file.name}`,
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
          .subscribe(event => {
            switch (event.type) {
              case HttpEventType.UploadProgress:
                const progress_pct = event.total ? (100 * (event.loaded / event.total)) : 0;
                this.dialogRef.componentInstance.data.progress_pct = progress_pct;
                console.log(`progress ${progress_pct}%`);
                break;
              case HttpEventType.Response:
                this.dialogRef.close();
                this.getFolderData();
                break;
            }
          });
      }
      // clear the selection
      fileSelect.value = '';
    }
  }

  dialogRef: any;
  openChooseUploadDialog() {
    this.dialogRef = this.dialog.open(ChooseUploadDialog);
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
  imports: [MatProgressBarModule, MatProgressSpinnerModule, DecimalPipe],
})
export class ProgressDialog {
  constructor(@Inject(MAT_DIALOG_DATA) public data: any) { }
};