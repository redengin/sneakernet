import { Component } from '@angular/core';
import { HttpClient, HttpParams } from '@angular/common/http';
import { retry } from 'rxjs';

import { KeyValuePipe } from '@angular/common';

import { NgIcon } from '@ng-icons/core';

import { Toolbar } from './components/toolbar';

// Types per openapi/catalog.yml
//==============================================================================
type Entry = {
  isFolder: boolean;
  // following only for file entries (i.e. isFolder = false)
  size?: BigInteger;
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
  imports: [Toolbar, NgIcon, KeyValuePipe],
  templateUrl: './app.component.html',
  styleUrl: './app.component.css'
})
export class AppComponent {
  constructor(private http: HttpClient) { }
  ngOnInit() { this.getFolderData(); }

  currentPath = '';
  folderData: Folder = {};

  getFolderData(): void {
    this.folderData = {};
    this.http.get<Folder>(`api/catalog/${this.currentPath}`)
      .pipe(
        retry({ delay: 500 /* ms */ }),
      )
      .subscribe(body => this.folderData = body);
  }

  chooseParentFolder(): void {
    this.currentPath = this.currentPath.split('/').slice(0, -1).join('/');
    this.getFolderData();
  }

  chooseSubfolder(subfolder: string): void {
    this.currentPath += `/${subfolder}`;
    this.getFolderData();
  }

  deleteFile(path: string, fileName: string): void {
    this.http.delete(`api/catalog/${path}/${fileName}`)
      .pipe(
        retry({ delay: 500 /* ms */ }),
      )
      .subscribe({
        complete: () => { this.getFolderData() },
        error: (error) => {
          // TODO raise up error dialog
          console.error(error);
        }
      })
  }

  addFile(path: string, event: Event): void {
    // get the files from the event target
    const fileSelect = event.target as HTMLInputElement;
    if (fileSelect.files) {
      // TODO raise up upload dialog
      const fileList: FileList = fileSelect.files;
      for (const file of fileList) {
        const timestamp = new Date(file.lastModified).toISOString();
        this.http.put(`api/catalog/${path}/${file.name}`,
          // data
          file.arrayBuffer,
          // additional options
          {
            headers: {
              'X-timestamp': timestamp,
            }
          }
        ).subscribe({
          complete: () => { this.getFolderData() },
          error: (error) => {
            // TODO raise up error dialog
            console.error(error);
          }
        });
      }
    }
  }
}

