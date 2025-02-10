import { Component } from '@angular/core';
import { HttpClient } from '@angular/common/http';
import { retry } from 'rxjs';

import { NgIcon } from '@ng-icons/core';

import { Toolbar } from './components/toolbar';

// Types per openapi/catalog.yml
//==============================================================================
type Folder = {
  locked?: boolean;
  subfolders?: string[];
  files?: File[];
};
type File = {
  name?: string;
  size?: BigInteger;
  timestamp?: string;
  title?: string;
  hasIcon?: boolean;
}

@Component({
  selector: 'app-root',
  imports: [ Toolbar, NgIcon ],
  templateUrl: './app.component.html',
  styleUrl: './app.component.css'
})
export class AppComponent {
  constructor(private http: HttpClient) { }
  ngOnInit() { this.getFolderData(); }

  currentPath = "";
  folderData: Folder = {};

  getFolderData() : void
  {
    this.folderData = {};
    this.http.get<Folder>(`api/catalog/${this.currentPath}`)
      .pipe(
        retry({ delay: 500 /* ms */ }),
      )
      .subscribe(body => this.folderData = body);
  }

  chooseParentFolder() : void
  {
    this.currentPath = this.currentPath.split('/').slice(0, -1).join('/');
    this.getFolderData();
  }

  chooseSubfolder(subfolder: string) : void
  {
    this.currentPath += `/${subfolder}`;
    this.getFolderData();
  }

  // delete file of current path
  deleteFile(name: string) : void
  {
    this.http.delete(`api/catalog/${this.currentPath}/${name}`)
      .pipe(
        retry({ delay: 500 /* ms */ }),
      )
      .subscribe( {
        complete: () => { this.getFolderData() },
        error: (error) => {
          console.error(error);
        }
      })
  }
}

