import * as jose from "https://deno.land/x/jose@v5.9.6/index.ts";
import { getUserByEmail } from "./supabase.ts";
import { SupabaseClient } from "@supabase/supabase-js";
import crypto from "node:crypto";
import { Buffer } from "node:buffer";

export const authenticateUser = async (
    supabaseClient: SupabaseClient,
    authToken: string,
): Promise<IUser> => {
    try {
        const jwtSecret = Deno.env.get("JWT_SECRET");

        if (!jwtSecret) throw new Error("JWT_SECRET not configured");

        console.log("jwtSecret", jwtSecret);
        const secretBytes = new TextEncoder().encode(jwtSecret);
        console.log("secretBytes", secretBytes);
        const payload = await jose.jwtVerify(authToken, secretBytes);
        console.log("payload", payload);

        const { payload: { email } } = payload;
        const user = await getUserByEmail(supabaseClient, email as string);
        return user;
    } catch (error: any) {
        throw new Error(error.message || "Failed to authenticate user");
    }
};

/**
 * Decrypts an encrypted secret with the same master encryption key.
 * @param encryptedData - base64 string from the database
 * @param iv - base64 IV from the database
 * @param masterKey - 32-byte string or buffer
 * @returns the original plaintext secret
 */
export function decryptSecret(
    encryptedData: string,
    iv: string,
    masterKey: string,
) {
    // Decode the base64 master key
    const decodedKey = Buffer.from(masterKey, "base64");
    if (decodedKey.length !== 32) {
        throw new Error(
            "ENCRYPTION_KEY must be 32 bytes when decoded from base64.",
        );
    }

    const decipher = crypto.createDecipheriv(
        "aes-256-cbc",
        decodedKey, // Use the decoded key instead of raw masterKey
        Buffer.from(iv, "base64"),
    );

    let decrypted = decipher.update(encryptedData, "base64", "utf8");
    decrypted += decipher.final("utf8");
    return decrypted;
}
