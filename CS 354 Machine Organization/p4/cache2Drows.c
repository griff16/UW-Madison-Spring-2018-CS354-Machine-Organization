int arr[3000][500];
int main(){
    int i, j;
    for(i=0;i<3000;i++){
        for(j=0;j<500;j++){
            arr[i][j] = i+j;
        }
    }

    return 0;
}
