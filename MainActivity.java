package com.example.apkencryptor;

import android.app.Activity;
import android.os.Bundle;
import android.content.Intent;
import android.net.Uri;
import android.view.View;
import android.widget.*;
import android.util.Log;
import java.io.*;
import javax.crypto.*;
import javax.crypto.spec.*;

public class MainActivity extends Activity {

    private static final int PICK_APK = 1001;
    private static final int REQ_PERM = 2001;

    // DEMO key/iv. Replace with stronger key management for real use.
    private static final byte[] AES_KEY = new byte[] {
        1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16
    };
    private static final byte[] AES_IV = new byte[] {
        0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15
    };

    private Button selectBtn;
    private TextView statusText;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        selectBtn = (Button) findViewById(R.id.selectBtn);
        statusText = (TextView) findViewById(R.id.statusText);

        selectBtn.setOnClickListener(new View.OnClickListener() {
				@Override
				public void onClick(View view) {
					if (checkSelfPermission(android.Manifest.permission.READ_EXTERNAL_STORAGE)
                        != android.content.pm.PackageManager.PERMISSION_GRANTED) {
						requestPermissions(new String[] {
											   android.Manifest.permission.READ_EXTERNAL_STORAGE,
											   android.Manifest.permission.WRITE_EXTERNAL_STORAGE
										   }, REQ_PERM);
						return;
					}
					openFilePicker();
				}
			});
    }

    @Override
    public void onRequestPermissionsResult(int requestCode, String[] permissions, int[] grantResults) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults);
        if (requestCode == REQ_PERM) {
            // naive: assume granted; if not, user will be asked again
            openFilePicker();
        }
    }

    private void openFilePicker() {
        Intent intent = new Intent(Intent.ACTION_GET_CONTENT);
        intent.setType("application/vnd.android.package-archive");
        intent.addCategory(Intent.CATEGORY_OPENABLE);
        try {
            startActivityForResult(Intent.createChooser(intent, "Select APK"), PICK_APK);
        } catch (Exception e) {
            statusText.setText("No file picker available");
            Log.e("APKEnc", "openFilePicker", e);
        }
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        if (requestCode == PICK_APK) {
            if (resultCode == RESULT_OK && data != null) {
                Uri uri = data.getData();
                if (uri != null) {
                    statusText.setText("Encrypting...");
                    try {
                        File out = encryptUriToFile(uri);
                        statusText.setText("Encrypted saved:\n" + out.getAbsolutePath());
                    } catch (Exception e) {
                        statusText.setText("Error: " + e.getMessage());
                        Log.e("APKEnc", "encrypt failed", e);
                    }
                } else {
                    statusText.setText("No file selected");
                }
            } else {
                statusText.setText("Selection cancelled");
            }
        } else {
            super.onActivityResult(requestCode, resultCode, data);
        }
    }

    /**
     * Reads the input URI and encrypts its bytes into an output file.
     * Returns the output File object.
     */
    private File encryptUriToFile(Uri inputUri) throws Exception {
        InputStream is = null;
        CipherOutputStream cos = null;
        FileOutputStream fos = null;
        try {
            is = getContentResolver().openInputStream(inputUri);
            if (is == null) throw new IOException("Cannot open selected file");

            File outDir = new File(getExternalFilesDir(null), "EncryptedAPKs");
            if (!outDir.exists()) outDir.mkdirs();

            String outName = "encrypted_" + System.currentTimeMillis() + ".apk.enc";
            File outFile = new File(outDir, outName);

            Cipher cipher = Cipher.getInstance("AES/CBC/PKCS5Padding");
            SecretKeySpec keySpec = new SecretKeySpec(AES_KEY, "AES");
            IvParameterSpec ivSpec = new IvParameterSpec(AES_IV);
            cipher.init(Cipher.ENCRYPT_MODE, keySpec, ivSpec);

            fos = new FileOutputStream(outFile);
            cos = new CipherOutputStream(fos, cipher);

            byte[] buf = new byte[8192];
            int r;
            while ((r = is.read(buf)) != -1) {
                cos.write(buf, 0, r);
            }
            cos.flush();

            return outFile;
        } finally {
            try { if (cos != null) cos.close(); } catch (IOException ignored) {}
            try { if (fos != null) fos.close(); } catch (IOException ignored) {}
            try { if (is != null) is.close(); } catch (IOException ignored) {}
        }
    }
}
